#if !defined(ARGS_H)
#define ARGS_H

#define INIT_SIZE 2

typedef struct ArgList{
    char **args;
    int size;
    int max;
} ArgList;

void InitArgs(ArgList *args);
void AddArg(ArgList *args, char *arg);
void FreeArgs(ArgList *args);

#endif // ARGS_H
