#if !defined(BUILTINS_H)
#define BUILTINS_H

#include "args.h"

void cd(ArgList *args);
void pwd(ArgList *args);
void echo(ArgList *args);
void repeat(ArgList *args);
void pinfo(ArgList *args);
void jobsc(ArgList *args);
void sig(ArgList *args);
void fg(ArgList *args);
#endif // BUILTINS_H
