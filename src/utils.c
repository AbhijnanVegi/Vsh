#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "utils.h"

//Error handling for irrecoverable errors
void throw_fatal_error()
{
    printf(RED);
    perror("Vsh");
    printf(RESET);
    exit(1);
}

//Error handling for recoverable errors
int check_and_throw_error(int return_code, int errno_code, char* message)
{
    if (return_code == errno_code)
    {
        if (message != NULL)
        {
            perror(message);
        }
        else perror(RED"Vsh"RESET);
    }
    return return_code;
}

/* Function that replaces tilda with home dir of shell
Caller must free the pointer returned */
char *replace_home(char *path)
{
    char *abs_path = malloc(sizeof(char) * (strlen(path) + homelen));
    if (path[0] == '~' && (strlen(path) == 1 || path[1] == '/'))
    {
        if (strlen(path) == 1)
        {
            strcpy(abs_path, home);
        }
        else
        {
            sprintf(abs_path, "%s%s", home, path + 1);
        }
    }
    else
    {
        strcpy(abs_path, path);
    }

    return abs_path;
}