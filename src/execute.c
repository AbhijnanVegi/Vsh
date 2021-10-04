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

int STDIN_FD;
int STDOUT_FD;

void reset_IO()
{
    reset_I();
    reset_O();
    close(STDIN_FD);
    close(STDOUT_FD);
}

void reset_I()
{
    if (dup2(STDIN_FD,STDIN_FILENO)<0)
        throw_fatal_error();
}

void reset_O()
{
    if (dup2(STDOUT_FD,STDOUT_FILENO)<0)
        throw_fatal_error();
}

// Parse args with IO redirection and execute
void execute(ArgList* args)
{
    ArgList* cargs = malloc(sizeof(ArgList));
    InitArgs(cargs);

    //Default IO Streams
    STDIN_FD = dup(STDIN_FILENO);
    STDOUT_FD = dup(STDOUT_FILENO);
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
            cargs = malloc(sizeof(ArgList));
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

            int fd = open(args->args[++i], O_RDONLY);
            if (fd < 0)
            {
                printf(RED"Vsh:"RESET" File '%s' not found\n", args->args[i + 1]);
                return;
            }

            dup2(fd, STDIN_FILENO);
        }
        else if (strcmp(args->args[i], ">") == 0)
        {
            if (i + 1 == args->size)
            {
                printf(RED"Vsh:"RESET" Missing file name after '>'\n");
                return;
            }

            int fd = open(args->args[++i], O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd < 0)
            {
                perror(RED"Vsh:"RESET);
                return;
            }

            dup2(fd, STDOUT_FILENO);
        }
        else if (strcmp(args->args[i], ">>") == 0)
        {
            if (i + 1 == args->size)
            {
                printf(RED"Vsh:"RESET" Missing file name after '>>'\n");
                return;
            }

            int fd = open(args->args[++i], O_WRONLY | O_CREAT | O_APPEND, 0644);
            if (fd < 0)
            {
                perror(RED"Vsh:"RESET);
                return;
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
void execute_command(ArgList *args, bool use_pipe)
{
    if (args->size == 0)
    {
        return;
    }

    int pipefd[2];
    if (use_pipe)
    {
        if(check_and_throw_error( pipe(pipefd) < 0, 1,NULL) == 1)
        {
            return;
        }
        dup2(pipefd[1], STDOUT_FILENO);
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
    else if (strcmp(args->args[0], "jobs") == 0)
    {
        jobsc(args);
    }
    else if (strcmp(args->args[0], "sig") == 0)
    {
        sig(args);
    }
    else if (strcmp(args->args[0], "fg") == 0)
    {
        fg(args);
    }
    else if (strcmp(args->args[0], "bg") == 0)
    {
        bg(args);
    }
    else if (strcmp(args->args[0], "replay") == 0)
    {
        replay(args);
    }
    else if (strcmp(args->args[0], "exit") == 0)
    {
        exit(0);
    }
    else
    {
        execute_external(args);
    }
    if (use_pipe)
    {
        close(pipefd[1]);
        dup2(pipefd[0], STDIN_FILENO);
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
        int status = execvp(args->args[0], args->args);
        if (status < 0)
        {
            reset_O();
            perror(RED"Vsh:"RESET);
        }
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
            add_job(pid, ((tmp = strrchr(args->args[0],'/')) == NULL)? args->args[0]:tmp + 1);
            waitpid(pid, &status, WUNTRACED);
            if (!WIFSTOPPED(status))
            {
            remove_job(pid);
            }
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
