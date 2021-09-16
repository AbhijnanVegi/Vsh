#if !defined(HISTORY_H_)
#define HISTORY_H
#include "args.h"

void init_history();
ArgList *read_history();
void write_history(char* line);
void history(ArgList *args);

#endif // HISTORY_H
