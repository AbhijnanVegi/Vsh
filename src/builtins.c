#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

#include "ls.h"
#include "args.h"
#include "utils.h"
#include "builtins.h"
#include "execute.h"
#include "prompt.h"

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
        check_and_throw_error(1, -1, "repeat: Too few arguments");
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
        check_and_throw_error(1, 1, "pinfo: Too many arguments");
        return;
    }

    char procPath[64];
    sprintf(procPath, "/proc/%d/stat", pid);

    FILE *procFile = fopen(procPath, "r");
    if (procFile == NULL)
    {
        printf(RED "pinfo: " RESET "%s\n", "No such process");
        return;
    }

    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    read = getline(&line, &len, procFile);
    if (read == -1)
    {
        printf(RED "pinfo: " RESET "%s\n", "No such process");
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