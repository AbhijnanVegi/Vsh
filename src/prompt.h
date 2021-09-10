#ifndef PROMPT_H
#define PROMPT_H

#define HOST_NAME_MAX 255
#define USER_NAME_MAX 255

char hostname[HOST_NAME_MAX];
char *username;
char *home;

void prompt();

#endif