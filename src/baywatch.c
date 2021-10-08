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
    ArgList *interrupts = parse_args(line);
    for (int i = 0; i < 5;i++)
    {
        getline(&line, &len, interrupt_file);
        interrupts = parse_args(line);
        if (strcmp(interrupts->args[interrupts->size-1], "i8042\n") == 0)
            break;
    }
    fclose(interrupt_file);
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

void newborn(int sleep_time)
{
    while(true)
    {
        FILE *newborn_file = fopen("/proc/loadavg", "r");
        if (newborn_file < 0)
        {
            perror("baywatch: ");
        }
        char *line = NULL;
        size_t len = 0;
        getline(&line, &len, newborn_file);
        fclose(newborn_file);
        ArgList *newborns = parse_args(line);
        printf("%s", newborns->args[4]);
        sleep(sleep_time);
    }
    exit(0);
}

void dirty(int sleep_time)
{
    while(true)
    {
        FILE *mem_file = fopen("/proc/meminfo", "r");
        if (mem_file < 0)
        {
            perror("baywatch: ");
        }
        char *line = NULL;
        size_t len = 0;
        for (int i = 0; i < 17; i++)
        {
            getline(&line, &len, mem_file);
        }
        fclose(mem_file);
        ArgList *data = parse_args(line);
        printf("%s %s", data->args[1], data->args[2]);
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
            else if (strcmp(args->args[i], "newborn") == 0)
            {
                cmd = &newborn;
            }
            else if (strcmp(args->args[i], "dirty") == 0)
            {
                cmd = &dirty;
            }
            else
            {
                printf("baywatch: Unknown command %s\n", args->args[i]);
                return;
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