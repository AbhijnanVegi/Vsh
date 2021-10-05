#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#include "args.h"
#include "rawio.h"
#include "baywatch.h"

void baywatch(ArgList *args)
{
    int interval = 5;
    char *cmd;
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
            cmd = strdup(args->args[i]);
        }
    }

    pid_t pid = fork();
    if (pid == 0)
    {
        while (1)
        {
            printf("Gaurdian reporting for duty\n");
            sleep(interval);
        }
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