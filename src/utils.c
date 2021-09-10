#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "utils.h"

void throw_fatal_error()
{
    printf(RED);
    perror("Vsh: ");
    printf(RESET);
    exit(1);
}