#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "args.h"
#include "execute.h"
#include "builtins.h"

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
}

