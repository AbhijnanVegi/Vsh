#if !defined(EXECUTE_H)
#define EXECUTE_H
#include "args.h"

void execute_command(ArgList *args);
void execute_external(ArgList *args);

#endif // EXECUTE_H