#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>

#include "ls.h"
#include "args.h"
#include "utils.h"
#include "builtins.h"
#include "execute.h"
#include "prompt.h"
#include "jobs.h"

char *OldDir;

void cd(ArgList *args)
{
    char *curr_dir = getcwd(NULL, 0);

    if (args->size == 2)
    {
        char *new_path = replace_home(args->args[1]);
        if (strcmp("-", args->args[1]) == 0)
        {
            if (OldDir == NULL)
            {
                OldDir = home;
            }
            new_path = strdup(OldDir);
        }
        int return_code = check_and_throw_error(chdir(new_path), -1, NULL);
        if (return_code == -1)
        {
            return;
        }
        free(new_path);
    }
    else if (args->size == 1)
    {
        int return_code = check_and_throw_error(chdir(home), -1, NULL);
        if (return_code == -1)
        {
            return;
        }
    }
    else
    {
        printf(RED "cd: " RESET "%s\n", "Too many arguments");
    }
    free(OldDir);
    OldDir = strdup(curr_dir);
    free(curr_dir);
}

void pwd(ArgList *args)
{
    char *cwd = getcwd(NULL, 0);
    printf("%s\n", cwd);
    free(cwd);
}

void echo(ArgList *args)
{
    for (int i = 1; i < args->size; i++)
    {
        printf("%s ", args->args[i]);
    }
    printf("\n");
}

void repeat(ArgList *args)
{
    if (args->size <= 2)
    {
        check_and_throw_error(1, -1, "repeat: Too few arguments\n");
    }
    char *ptr;
    long n = strtol(args->args[1], &ptr, 10);
    if (ptr[0] != '\0')
    {
        printf(RED "repeat: " RESET "Usage: repeat <int> command\n");
        return;
    }
    ArgList *subcmd = malloc(sizeof(ArgList));
    InitArgs(subcmd);
    for (int i = 2; i < args->size; i++)
    {
        AddArg(subcmd, args->args[i]);
    }
    for (int i = 0; i < n; i++)
    {
        execute(subcmd);
    }
    FreeArgs(subcmd);
}

void pinfo(ArgList *args)
{
    pid_t pid;

    if (args->size == 1)
    {
        pid = getpid();
    }
    else if (args->size == 2)
    {
        char *ptr;
        pid = strtol(args->args[1], &ptr, 10);
        if (ptr[0] != '\0')
        {
            printf(RED "pinfo: " RESET "Usage: pinfo ?<int> \n");
            return;
        }
    }
    else
    {
        check_and_throw_error(1, 1, "pinfo: Too many arguments\n");
        return;
    }

    char procPath[64];
    sprintf(procPath, "/proc/%d/stat", pid);

    FILE *procFile = fopen(procPath, "r");
    if (procFile == NULL)
    {
        printf(RED "pinfo: " RESET "%s\n", "No such process\n");
        return;
    }

    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    read = getline(&line, &len, procFile);
    if (read == -1)
    {
        printf(RED "pinfo: " RESET "%s\n", "No such process\n");
        return;
    }

    printf("pid -> %d\n", pid); // Print pid of process

    char *value;
    char *storage;
    int counter = 1;
    int pgrp;
    value = strtok_r(line, " ", &storage);
    while (value != NULL)
    {
        if (counter == 3)
        {
            printf("Process Status -> %s", value);
        }
        else if (counter == 5)
        {
            pgrp = atoi(value);
        }
        else if (counter == 8)
        {
            if (pgrp == atoi(value))
            {
                printf("+");
            }
            printf("\n");
        }
        else if (counter == 23)
        {
            printf("Memory -> %s B\n", value);
            break;
        }
        value = strtok_r(NULL, " ", &storage);
        counter++;
    }

    fclose(procFile);
    char exeProc[64];
    sprintf(exeProc, "/proc/%d/exe", pid);

    char *exePath = malloc(sizeof(char) * 257);
    read = readlink(exeProc, exePath, 256);
    exePath[read] = '\0';
    format_directory(&exePath);

    if (read == -1)
    {
        printf("Executable Path -> NULL\n");
        return;
    }

    printf("Executable Path -> %s\n", exePath);
    free(line);
    return;
}

void jobsc(ArgList *args)
{
    bool s, r;
    if (args->size == 1)
    {
        s = true;
        r = true;
    }
    else if (args->size == 2)
    {
        if (strcmp(args->args[1], "-s") == 0)
        {
            s = true;
            r = false;
        }
        else if (strcmp(args->args[1], "-r") == 0)
        {
            s = false;
            r = true;
        }
        else
        {
            printf(RED "jobsc:" RESET " Usage: jobsc [-s|-r]");
            return;
        }
    }
    else
    {
        printf(RED "jobsc:" RESET " Usage: jobsc [-s|-r]");
        return;
    }

    job *walk = jobs->next;
    for (int i = 1; walk != NULL; i++)
    {
        char procPath[64];
        sprintf(procPath, "/proc/%d/stat", walk->pid);

        FILE *procFile = fopen(procPath, "r");
        if (procFile == NULL)
        {
            printf(RED "pinfo: " RESET "%s\n", "No such process\n");
            return;
        }

        char *line = NULL;
        size_t len = 0;
        ssize_t read;

        read = getline(&line, &len, procFile);

        char status[33] = "Unknown";
        char *value;
        char *storage;
        int counter = 1;

        value = strtok_r(line, " ", &storage);
        while (value != NULL)
        {
            if (counter == 3)
            {
                if (strcmp(value, "R") == 0)
                {
                    strcpy(status, "Running");
                }
                else if (strcmp(value, "S") == 0)
                {
                    strcpy(status, "Stopped");
                }
                else if (strcmp(value, "T") == 0)
                {
                    strcpy(status, "Stopped");
                }
                else if (strcmp(value, "Z") == 0)
                {
                    strcpy(status, "Zombie");
                }
                else
                {
                    strcpy(status, "Unknown");
                }
                break;
            }
            value = strtok_r(NULL, " ", &storage);
            counter++;
        }
        if (s && status[0] == 'S' || r && status[0] == 'R')
        {
            printf("[%d] %s %s [%d]\n", i, status, walk->name, walk->pid);
        }
        walk = walk->next;
    }
}

void sig(ArgList *args)
{
    if (args->size != 3)
    {
        printf(RED "sig:" RESET " Usage: sig <pid> <signal>\n");
        return;
    }

    pid_t pid;
    int pidi;
    int sig;
    char *ptr;
    pidi = strtol(args->args[1], &ptr, 10);
    if (ptr[0] != '\0')
    {
        printf(RED "sig:" RESET " Usage: sig <pid> <signal>\n");
        return;
    }
    sig = strtol(args->args[2], &ptr, 10);
    if (ptr[0] != '\0')
    {
        printf(RED "sig:" RESET " Usage: sig <pid> <signal>\n");
        return;
    }

    pid = get_job(pidi);
    if (pid == -1)
    {
        printf(RED "sig:" RESET " No such process\n");
        return;
    }
    check_and_throw_error(kill(pid, sig), -1, "sig: ");
}

void fg(ArgList *args)
{
    if (args->size != 2)
    {
        printf(RED "fg:" RESET " Usage: fg <pid>\n");
        return;
    }

    pid_t pid;
    int pidi;
    char *ptr;
    pidi = strtol(args->args[1], &ptr, 10);
    if (ptr[0] != '\0')
    {
        printf(RED "fg:" RESET " Usage: fg <pid>\n");
        return;
    }

    pid = get_job(pidi);
    if (pid == -1)
    {
        printf(RED "fg:" RESET " No such process\n");
        return;
    }
    int status;
    check_and_throw_error(kill(pid, SIGCONT), -1, "fg: ");
    signal(SIGTTOU, SIG_IGN);
    tcsetpgrp(0, pid);
    waitpid(pid, &status, WUNTRACED);
    if (!WIFSTOPPED(status))
    {
        remove_job(pid);
    }
    tcsetpgrp(0, getpgrp());
    signal(SIGTTOU, SIG_DFL);
}

void bg(ArgList *args)
{
    if (args->size != 2)
    {
        printf(RED "bg:" RESET " Usage: bg <pid>\n");
        return;
    }

    pid_t pid;
    int pidi;
    char *ptr;
    pidi = strtol(args->args[1], &ptr, 10);
    if (ptr[0] != '\0')
    {
        printf(RED "bg:" RESET " Usage: bg <pid>\n");
        return;
    }

    pid = get_job(pidi);
    if (pid == -1)
    {
        printf(RED "bg:" RESET " No such process\n");
        return;
    }
    int status;
    check_and_throw_error(kill(pid, SIGCONT), -1, "bg: ");
}

void replay(ArgList *args)
{
    bool command = false, interval = false, period = false;
    bool reading_command = false;
    int interval_time = 0, period_time = 0;
    ArgList *subcmd = malloc(sizeof(ArgList));
    InitArgs(subcmd);
    if (args->size < 7)
    {
        printf(RED "replay:" RESET " Usage: replay -command <command> -interval <time> -period <time>\n");
        return;
    }
    for (int i = 1; i < args->size; i++)
    {
        if (strcmp(args->args[i], "-command") == 0)
        {
            reading_command = true;
            command = true;
        }
        else if (strcmp(args->args[i], "-interval") == 0)
        {
            reading_command = false;
            interval = true;
            i++;
            char *ptr;
            interval_time = strtol(args->args[i], &ptr, 10);
            if (ptr[0] != '\0')
            {
                printf(RED "replay:" RESET " Usage: replay -command <command> -interval <time> -period <time>\n");
                return;
            }
        }
        else if (strcmp(args->args[i], "-period") == 0)
        {
            reading_command = false;
            period = true;
            i++;
            char *ptr;
            period_time = strtol(args->args[i], &ptr, 10);
            if (ptr[0] != '\0')
            {
                printf(RED "replay:" RESET " Usage: replay -command <command> -interval <time> -period <time>\n");
                return;
            }
        }
        else if (reading_command)
        {
            AddArg(subcmd, args->args[i]);
        }
        else
        {
            printf(RED "replay:" RESET " Usage: replay -command <command> -interval <time> -period <time>\n");
            return;
        }
    }
    if (!(command && interval && period))
    {
        printf(RED "replay:" RESET " Usage: replay -command <command> -interval <time> -period <time>\n");
        return;
    }

    for (int i = 0; i < period_time/interval_time; i++)
    {
        sleep(period_time);
        execute_command(subcmd, false);
    }
}