#if !defined(JOBS_H)
#define JOBS_H

#include <unistd.h>

typedef struct job {
    pid_t pid;
    char *name;
    struct job *next;
} job;

extern job *jobs;
void init_jobs();
void add_job(int pid, char *name);
void remove_job(int pid);
char *get_job_name(int pid);
void child_handler(int sig);
void init_child_handler();
pid_t get_job(int index);
#endif // JOBS_H
