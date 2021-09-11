#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>

#include "prompt.h"
#include "parse.h"
#include "utils.h"

char *line = NULL;
size_t len = 0;
ssize_t nread;

char *home = NULL;
int homelen = 0;

int main()
{
    char *cwd = getcwd(NULL, 0);
    
    if (home == NULL)
    {
        homelen = strlen(cwd);
        home = malloc(sizeof(char) * homelen);
        strcpy(home, cwd);
    }

    while (true)
    {
        prompt();
        ssize_t cmd = getline(&line, &len, stdin);
        if (cmd == EOF || (strcmp(line, "exit\n") == 0))
        {
            break;
        }

        parse(line);
    }
    free(line);
}