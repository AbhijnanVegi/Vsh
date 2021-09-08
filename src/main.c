#include "prompt.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

char *line = NULL;
size_t len = 0;
ssize_t nread;

int main()
{
    while (true)
    {
        prompt();
        ssize_t cmd = getline(&line, &len, stdin);
        if (cmd == EOF)
        {
            exit(0);
        }
    }
}