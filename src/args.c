#include <stdlib.h>
#include <string.h>

#include "args.h"
#include "utils.h"

void InitArgs(ArgList *args) 
{
    args->args = malloc(sizeof(char *) * INIT_SIZE);
    args->size = 0;
    args->max = INIT_SIZE;

    if (args->args == NULL) {
        throw_fatal_error();
    }
    args->args[0] = NULL;
}

void AddArg(ArgList *args, char *arg)
{
    if (args->size == args->max) {
        args->max *= 2;
        args->args = realloc(args->args, sizeof(char *) * args->max);
        if (args->args == NULL) {
            throw_fatal_error();
        }
    }
    char* new_arg = malloc(sizeof(char) * (strlen(arg) + 1));
    if (new_arg == NULL) {
        throw_fatal_error();
    }
    strcpy(new_arg, arg);
    args->args[args->size] = new_arg;
    args->size++;
}

void FreeArgs(ArgList *args)
{
    for (int i = 0; i < args->size; i++) {
        free(args->args[i]);
    }
    free(args->args);
}