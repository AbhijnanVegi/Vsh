#if !defined(EXECUTE_H)
#define EXECUTE_H

#include <stdbool.h>
#include "args.h"

void reset_IO();
void reset_I();
void reset_O();

void execute(ArgList *args);
void execute_command(ArgList *args, bool use_pipe);
void execute_external(ArgList *args);

#endif // EXECUTE_H
