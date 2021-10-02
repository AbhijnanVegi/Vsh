#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <wait.h>

#include "jobs.h"
#include "prompt.h"
#include "rawio.h"

job *jobs;

void init_jobs()
{
    jobs = malloc(sizeof(job));
    jobs->pid = 0;
    jobs->next = NULL;
}

void add_job(int pid, char* name)
{
    struct job* new_job = malloc(sizeof(struct job));
    new_job->pid = pid;
    new_job->name = strdup(name);
    new_job->next = NULL;
    job* walk = jobs;
    while (walk->next != NULL)
    {
        walk = walk->next;
    }
    walk->next = new_job;
}

void remove_job(int pid)
{
    job* walk = jobs;
    job* prev = jobs;
    while (walk != NULL)
    {
        if (walk->pid == pid)
        {
            prev->next = walk->next;
        }
        prev = walk;
        walk = walk->next;
    }
}

char* get_job_name(int pid)
{
    job* walk = jobs;
    while (walk != NULL)
    {
        if (walk->pid == pid)
        {
            return walk->name;
        }
        walk = walk->next;
    }
    return NULL;
}

void child_handler(int signum)
{
    int status;
    pid_t pid;
    while ((pid = waitpid(-1, &status, WNOHANG | WUNTRACED )) > 0)
    {
        char* name = get_job_name(pid);
        if (name == NULL)
        {
            continue;
        }

        if (WIFEXITED(status)) {
            printf("\n%s with pid %d exited normally\n",name, pid);
            remove_job(pid);
        }else if (WIFSTOPPED(status)) {
            printf("%s with pid %d suspended normally\n",name, pid);
        }else {
            printf("%s with pid %d did not exit normally\n",name, pid);
            remove_job(pid);
        }
        prompt();
        printf("%s", inp);
    }
}

void init_child_handler()
{
    struct sigaction sa;
    sa.sa_handler = child_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sigaction(SIGCHLD, &sa, NULL);
}

pid_t get_job(int index)
{
    job *walk = jobs->next;
    for (int i = 1; i <= index; i++)
    {
        if (walk == NULL)
        {
            return -1;
        }
        if (i == index)
        {
            return walk->pid;
        }
        walk = walk->next;
    }
    return -1;
}