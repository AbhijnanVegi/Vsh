#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "args.h"
#include "utils.h"
#include "builtins.h"

char *OldDir;

void cd(ArgList *args)
{   
    char* curr_dir = getcwd(NULL, 0);

    if (args->size == 2)
    {
        char *new_path = replace_home(args->args[1]);
        if (strcmp("-", args->args[1]) == 0)
        {
            if (OldDir == NULL)
            {
                OldDir = home;
            }
            new_path = OldDir;
        }
        int return_code = check_and_throw_error(chdir(new_path), -1);
        free(new_path);
    }
    else if (args->size == 1)
    {
        int return_code = check_and_throw_error(chdir(home), -1);
    }
    else
    {
        printf(RED"cd: "RESET"%s\n", "Too many arguments");
    }

    OldDir = curr_dir;
}

void pwd(ArgList *args)
{
    char *cwd = getcwd(NULL, 0);
    printf("%s\n", cwd);
}

void echo(ArgList *args)
{
    for (int i = 1; i < args->size; i++)
    {
        printf("%s ", args->args[i]);
    }
    printf("\n");
}
