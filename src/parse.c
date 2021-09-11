#include <string.h>
#include <stdlib.h>

#include "utils.h"
#include "args.h"
#include "execute.h"

ArgList* parse_args(char* token)
{
    ArgList* args = malloc(sizeof(ArgList));
    if (args == NULL)
    {
        throw_fatal_error();
    }
    InitArgs(args);

    char* arg = strtok(token, " \t");
    while (arg != NULL)
    {
        AddArg(args, arg);
        arg = strtok(NULL, " \t");
    }

    return args;
}

void parse(char *line)
{
    char *token_storage;
    char *token = strtok_r(line, ";\n", &token_storage);

    while (token != NULL)
    {
        ArgList* args = parse_args(token);
        execute_command(args);

        token = strtok_r(NULL, ";\n", &token_storage);
    }

}