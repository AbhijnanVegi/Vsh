#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

#include "args.h"
#include "execute.h"
#include "builtins.h"
#include "utils.h"
#include "ls.h"
#include "jobs.h"

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
    else if (strcmp(args->args[0], "exit") == 0)
    {
        exit(0);
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
        signal(SIGINT, SIG_DFL);
        signal(SIGTSTP, SIG_DFL);

        for (int i = 0; i < args->size; i++)
        {
            args->args[i] = replace_home(args->args[i]);
        }

        setpgid(0, 0);

        AddArg(args, NULL);
        int status = check_and_throw_error(execvp(args->args[0], args->args), -1, NULL);
        exit(status);
    }
    else
    {
        int status;
        if (!bg)
        {   
            signal(SIGTTOU, SIG_IGN);
            tcsetpgrp(0, pid);
            waitpid(pid, &status, WUNTRACED);
            tcsetpgrp(0, getpgrp());
            signal(SIGTTOU, SIG_DFL);
        }
        else
        {
            printf("%s -> pid:%d\n", args->args[0], pid);
            add_job(pid, args->args[0]);
        }
    }
}
