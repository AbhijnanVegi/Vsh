#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pwd.h>

#include "args.h"
#include "utils.h"
#include "history.h"

char *history_file;

void init_history()
{
    char *home_dir = getpwuid(getuid())->pw_dir;
    history_file = malloc(strlen(home_dir) + strlen("/.history") + 1);
    sprintf(history_file, "%s/.vsh_history", home_dir);
}

ArgList *read_history()
{
    FILE *fp = fopen(history_file, "r");
    if (fp == NULL)
        return NULL;
    ArgList *history = malloc(sizeof(ArgList));
    InitArgs(history);
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, fp)) != -1)
    {
        AddArg(history, line);
    }
    fclose(fp);
    free(line);
    return history;
}

void write_history(char *line)
{
    ArgList *history = read_history();
    int linelen = strlen(line);
    if (line[linelen] != '\n')
    {
        line = realloc(line, linelen + 2);
        line[linelen] = '\n';
        line[linelen + 1] = '\0';
    }

    if (history != NULL && history->size > 0 && strcmp(history->args[0], line) == 0)
    {
        return;
    }
    else if (line[0] == '\n')
        return;
    FILE *fp = fopen(history_file, "w");
    if (fp == NULL)
        throw_fatal_error();

    fprintf(fp, "%s", line);

    if (history != NULL)
    {
        for (int i = 0; i < (19 <= history->size ? 19 : history->size); i++)
        {
            fprintf(fp, "%s", history->args[i]);
        }
    }
    fclose(fp);
    if (history != NULL)
        FreeArgs(history);
}

void history(ArgList *args)
{
    int to_write;
    if (args->size == 1)
    {
        to_write = 10;
    }
    else if (args->size == 2)
    {
        char *ptr;
        long to_write = strtol(args->args[1], &ptr, 10);
        if (ptr[0] != '\0')
        {
            printf(RED "history: " RESET "Usage: history ?<num>\n");
            return;
        }
    }
    else
    {
        printf(RED "History:" RESET "Usage history ?<num>");
        return;
    }
    ArgList *history = read_history();
    to_write = to_write <= history->size ? to_write : history->size;
    for (int i = to_write - 1; i >= 0; i--)
    {
        printf("%s", history->args[i]);
    }
    FreeArgs(history);
}