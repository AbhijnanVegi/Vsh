#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pwd.h>

#include "prompt.h"
#include "utils.h"

#define HOST_NAME_MAX 255
#define USER_NAME_MAX 255

char hostname[HOST_NAME_MAX];
char *username;

void prompt() {

    gethostname(hostname, HOST_NAME_MAX);
    username  = getpwuid(getuid())->pw_name;

    char* cwd = getcwd(NULL, 0);
    printf(GREEN"[%s@%s "RESET"%s"GREEN"]$ "RESET , username,hostname,cwd);
}