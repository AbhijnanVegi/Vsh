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
#include "history.h"

void reset_IO(int STDIN_FD, int STDOUT_FD)
{
    reset_I(STDIN_FD);
    reset_O(STDOUT_FD);
}

void reset_I(int STDIN_FD)
{
    if (dup2(STDIN_FD,STDIN_FILENO)<0)
        throw_fatal_error();
    close(STDIN_FD);
}

void reset_O(int STDOUT_FD)
{
    if (dup2(STDOUT_FD,STDOUT_FILENO)<0)
    throw_fatal_error();
    close(STDOUT_FD);
}

// Parse args with IO redirection and execute
void execute(ArgList* args)
{
    ArgList* cargs = malloc(sizeof(ArgList));
    InitArgs(cargs);

    //Default IO Streams
    int STDIN_FD = dup(STDIN_FILENO);
    int STDOUT_FD = dup(STDOUT_FILENO);
    if (check_and_throw_error(STDIN_FD<0||STDOUT_FD<0,1,NULL))
    {
        return;
    }

    for (int i = 0; i < args->size; i++)
    {
        if (strcmp(args->args[i], "|") == 0)
        {
            execute_command(cargs, true);
            FreeArgs(cargs);
            InitArgs(cargs);

            // Reset STDOUT changed by execute_command when piping
           reset_O(STDOUT_FD);
        }
        else if (strcmp(args->args[i], "<") == 0)
        {
            if (i + 1 == args->size)
            {
                printf(RED"Vsh:"RESET" Missing file name after '<'\n");
                return;
            }

            int fd = open(args->args[i + 1], O_RDONLY);
            if (fd < 0)
            {
                printf(RED"Vsh:"RESET" File '%s' not found\n", args->args[i + 1]);
                return;
            }

            dup2(fd, STDIN_FILENO);
        }
        else
        {
            AddArg(cargs, args->args[i]);
        }
    }
    execute_command(cargs, false);
    reset_IO(STDIN_FD, STDOUT_FD);
}


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
    else if (strcmp(args->args[0], "history") == 0)
    {
        history(args);
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
        char* tmp;
        if (!bg)
        {   
            setpgid(pid,0);
            signal(SIGTTOU, SIG_IGN);
            tcsetpgrp(0, pid);
            waitpid(pid, &status, WUNTRACED);
            tcsetpgrp(0, getpgrp());
            signal(SIGTTOU, SIG_DFL);
        }
        else
        {
            printf("%s -> pid:%d\n", args->args[0], pid);
            add_job(pid, ((tmp = strrchr(args->args[0],'/')) == NULL)? args->args[0]:tmp + 1);
        }
    }
}
