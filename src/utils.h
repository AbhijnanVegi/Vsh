#ifndef UTILS_H
#define UTILS_H

#define GREEN "\033[0;32m"
#define RED "\033[0;31m"
#define YELLOW "\033[0;33m"
#define BLUE "\033[0;34m"
#define MAGENTA "\033[0;35m"
#define CYAN "\033[0;36m"
#define WHITE "\033[0;37m"
#define RESET "\033[0m"

extern char *home;
extern size_t homelen;

void throw_fatal_error();
char* replace_home(char *path);
int check_and_throw_error(int return_code, int errno_code, char* message);

#endif