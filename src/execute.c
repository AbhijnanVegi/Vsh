#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>

#include "args.h"
#include "execute.h"
#include "builtins.h"
#include "utils.h"
#include "ls.h"

// Function: execute
void execute_command(ArgList *args)
{
    if (args->size == 0)
    {
        return;
    }

    if (strcmp(args->args[0], "cd") == 0)
    {
        cd(args);
    }
    else if (strcmp(args->args[0], "pwd") == 0)
    {
        pwd(args);
    }
    else if (strcmp(args->args[0], "echo") == 0)
    {
        echo(args);
    }
    else if (strcmp(args->args[0], "ls") == 0)
    {
        ls(args);
    }
    else if (strcmp(args->args[0], "repeat") == 0)
    {
        repeat(args);
    }
    else if (strcmp(args->args[0], "pinfo") == 0)
    {
        pinfo(args);
    }
    else
    {
        execute_external(args);
    }
}

void execute_external(ArgList *args)
{
    pid_t pid = fork();
    bool bg = false;

    if (strcmp(args->args[args->size - 1], "&") == 0)
    {
        bg = true;
        args->size--;
    }

    if (pid == 0)
    {
        for (int i = 0; i < args->size; i++)
        {
            args->args[i] = replace_home(args->args[i]);
        }
        AddArg(args, NULL);
        int status = check_and_throw_error(execvp(args->args[0], args->args), -1, NULL);
        exit(status);
    }
    else
    {
        int status;
        if (!bg)
            waitpid(pid, &status, WUNTRACED);
        else
        {
            printf("%s -> pid:%d\n", args->args[0], pid);
        }
    }
}
