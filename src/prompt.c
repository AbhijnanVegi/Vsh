#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pwd.h>

#include "prompt.h"
#include "utils.h"

char hostname[HOST_NAME_MAX];
char *username;
char *home = NULL;
int homelen = 0;

void format_directory(char **directory)
{
    if (strlen(*directory) < homelen)
    {
        return;
    }
    else
    {
        for (int i = 0; i < homelen; i++)
        {
            if (home[i] != (*directory)[i])
            {
                return;
            }
        }
    }
    *(*directory + homelen - 1) = '~';
    *directory = *directory + homelen - 1;
}

void prompt()
{

    gethostname(hostname, HOST_NAME_MAX);
    username = getpwuid(getuid())->pw_name;

    char *cwd = getcwd(NULL, 0);
    if (home == NULL)
    {
        homelen = strlen(cwd);
        home = malloc(sizeof(char) * homelen);
        strcpy(home, cwd);
    }

    format_directory(&cwd);
    printf(GREEN "[%s@%s " RESET "%s" GREEN "]$ " RESET, username, hostname, cwd);
}