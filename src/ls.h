#if !defined(LS_H)
#define LS_H
#include <stdbool.h>
#include "args.h"

void ls(ArgList *args);
void print_ls(char *path, bool l, bool a);
void file_details(char *path);
#endif // LS_H
