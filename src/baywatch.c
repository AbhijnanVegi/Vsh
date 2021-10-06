#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <stdbool.h>

#include "args.h"
#include "parse.h"
#include "rawio.h"
#include "baywatch.h"

void _interrupt(void)
{
    FILE *interrupt_file = fopen("/proc/interrupts", "r");
    if (interrupt_file < 0)
    {
        perror("baywatch: ");
    }
    char *line = NULL;
    size_t len = 0;
    for (int i = 0; i < 3;i++)
    {
        getline(&line, &len, interrupt_file);
    }
    fclose(interrupt_file);
    ArgList *interrupts = parse_args(line);
    for (int i = 1; i < interrupts->size - 3; i++)
    {
        printf("%s\t", interrupts->args[i]);
    }
    printf("\n");
}

void interrupt(int sleep_time)
{
    FILE* interrupt_file = fopen("/proc/interrupts", "r");
    if (interrupt_file < 0) {
        perror("baywatch: ");
    }
    char *line = NULL;
    size_t len = 0;
    getline(&line, &len, interrupt_file);
    fclose(interrupt_file);

    ArgList *headers = parse_args(line);
    for (int i = 0; i < headers->size-1; i++)
    {
        printf("%s\t", headers->args[i]);
    }
    printf("\n");
    while (true)
    {
        _interrupt();
        sleep(sleep_time);
    }
    exit(0);
}

void baywatch(ArgList *args)
{
    int interval = 5;
    void (*cmd)(int);
    if (args->size != 4)
    {
        printf("Usage: baywatch -n <interval> <command>\n");
        return;
    }
    for (int i = 1; i < args->size; i++)
    {
        if (strcmp(args->args[i], "-n") == 0)
        {
            char *ptr;
            if (i + 1 == args->size)
            {
                printf("Usage: baywatch -n <interval> <command>\n");
                return;
            }
            interval = strtol(args->args[i + 1], &ptr, 10);

            if (ptr[0] != '\0' || interval < 0)
            {
                printf("Usage: baywatch -n <interval> <command>\n");
                return;
            }
            i++;
        }
        else
        {
            if (strcmp(args->args[i], "interrupt") == 0)
            {
                cmd = &interrupt;
            }
        }
    }

    pid_t pid = fork();
    if (pid == 0)
    {
        cmd(interval);
    }
    else
    {
        char c;
        setbuf(stdout, NULL);
        enableRawMode();
        while (read(STDIN_FILENO, &c, 1) == 1)
        {
            if (c == 'q')
            {
                kill(pid, SIGTERM);
                break;
            }
        }
        disableRawMode();
    }
}