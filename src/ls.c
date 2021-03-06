#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

#include "builtins.h"
#include "args.h"
#include "utils.h"
#include "ls.h"

void ls(ArgList *args)
{
    bool l = false, a = false;
    ArgList *dirs = malloc(sizeof(ArgList));
    InitArgs(dirs);
    for (int i = 1; i < args->size; i++)
    {
        if ((args->args[i][0] ==  '-') && strlen(args->args[i]) > 1)
        {
            for (size_t j = 1; j < strlen(args->args[i]); j++)
            {
                if (args->args[i][j] == 'l')
                    l = true;
                else if (args->args[i][j] == 'a')
                    a = true;
                else
                {
                    printf(RED"ls:"RESET" invalid option -- '%c'\n", args->args[i][j]);
                    return;
                }
            }
        }
        else
        {
            AddArg(dirs, args->args[i]);
        }
    }

    if (dirs->size == 0)
    {
        print_ls(".",l,a);
    }
    else if (dirs->size == 1)
    {
        print_ls(replace_home(dirs->args[0]),l,a);
    }
    else
    {
        for (int i = 0; i < dirs->size; i++)
        {
            printf(YELLOW"%s:\n"RESET, dirs->args[i]);
            print_ls(replace_home(dirs->args[i]),l,a);
        }
    }
    FreeArgs(dirs);
}

void print_ls(char *path, bool l, bool a)
{
    DIR *dir = opendir(path);
    if (dir == NULL)
    {
        // Check if file
        struct stat s;
        if (stat(path, &s) == 0)
        {
            if (S_ISREG(s.st_mode))
            {
                if (l)
                {
                    file_details(path);
                }
                char* filename = strrchr(path, '/');
                if (filename == NULL)
                    filename = path;
                printf("%s\n", filename);
                return;
            }
        }
        
        char *message = malloc(sizeof(char) * (strlen(path) + strlen("ls: cannot access ''") + 1));
        sprintf(message, "ls: cannot access '%s'", path);
        check_and_throw_error(0, 0, message);
        free(message);
        return;
    }

    struct dirent *entry;
    if (l)
    {
        int block_count = 0;
        while ((entry = readdir(dir)) != NULL)
        {
            if (!a && entry->d_name[0] == '.')
                continue;
            char* file = malloc(sizeof(char) * (strlen(entry->d_name) + strlen(path) + 2));
            sprintf(file, "%s/%s", path, entry->d_name);
            struct stat s;
            if (lstat(file, &s) == -1) continue;
            block_count += s.st_blocks;
            free(file);
        }
        printf("total %d\n", block_count/2);
        rewinddir(dir);
    }

    while ((entry = readdir(dir)) != NULL)
    {
        if (!a && entry->d_name[0] == '.')
            continue;

        if (l)
        {
            char* file = malloc(strlen(path) + strlen(entry->d_name) + 2);
            sprintf(file, "%s/%s", path, entry->d_name);
            file_details(file);
            free(file);
        }
        printf("%s\n", entry->d_name);
        
    }
    closedir(dir);
}

// Returns char of file type
int file_type_letter(mode_t mode) {
    char c;
    if (S_ISREG(mode))
        c = '-';
    else if (S_ISDIR(mode))
        c = 'd';
    else if (S_ISBLK(mode))
        c = 'b';
    else if (S_ISCHR(mode))
        c = 'c';
    else if (S_ISFIFO(mode))
        c = 'p';
    else if (S_ISLNK(mode))
        c = 'l';
    else if (S_ISSOCK(mode))
        c = 's';
    else {
        c = '?';
    }
    return (c);
}

// Prints details for a file in ls format
void file_details(char *path)
{
    struct stat s;
    if (lstat(path, &s) == -1)
    {
        char *message = malloc(sizeof(char) * (strlen(path) + strlen("ls: cannot access ''") + 1));
        sprintf(message, "ls: cannot access '%s'", path);
        check_and_throw_error(0, 0, message);
        free(message);
        return;
    }

    char *rwx[] = {"---", "--x", "-w-", "-wx","r--", "r-x", "rw-", "rwx"};
    static char modes[11];

    modes[0] = file_type_letter(s.st_mode);
    strcpy(&modes[1], rwx[(s.st_mode >> 6) & 7]);
    strcpy(&modes[4], rwx[(s.st_mode >> 3) & 7]);
    strcpy(&modes[7], rwx[(s.st_mode & 7)]);
    // Special file mode handling
    if (s.st_mode & S_ISUID)
        modes[3] = (s.st_mode & S_IXUSR) ? 's' : 'S';
    
    if (s.st_mode & S_ISGID)
        modes[6] = (s.st_mode & S_IXGRP) ? 's' : 'l';
    
    if (s.st_mode & S_ISVTX)
        modes[9] = (s.st_mode & S_IXOTH) ? 't' : 'T';
    modes[10] = '\0';

    //print stuff
    char date[21];
    printf("%s ", modes);
    printf("%3.ld ", s.st_nlink);
    printf("%s ", getpwuid(s.st_uid)->pw_name);
    printf("%s ", getgrgid(s.st_gid)->gr_name);
    printf("%6.ld ", s.st_size);
    if (time(0) - s.st_mtime < 15780000)
    strftime(date, 20, "%b %d %H:%M", localtime(&(s.st_mtime)));
    else
    strftime(date, 20, "%b %d  %Y", localtime(&(s.st_mtime)));
    printf("%s ", date);
    return;
}