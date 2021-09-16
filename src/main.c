#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <signal.h>

#include "prompt.h"
#include "parse.h"
#include "utils.h"
#include "jobs.h"
#include "history.h"

char *line = NULL;
size_t len = 0;
ssize_t nread;

char *home = NULL;
int homelen = 0;

int main()
{
    char *cwd = getcwd(NULL, 0);
    
    //Init jobs
    init_jobs();

    // Ignore ctrl+C ctrl+Z
    signal(SIGINT, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);
    
    //Handle SIGCHLD
    signal(SIGCHLD, child_handler);

    // Init History
    init_history();

    // Set home directory
    if (home == NULL)
    {
        homelen = strlen(cwd);
        home = malloc(sizeof(char) * homelen);
        strcpy(home, cwd);
        free(cwd);
    }

    while (true)
    {
        prompt();
        ssize_t cmd = getline(&line, &len, stdin);
        if (cmd == EOF)
        {
            break;
        }
        write_history(line);
        parse(line);
    }
    free(line);
    free(home);
}
