//
// Created by vegi on 18/09/21.
//

#ifndef VSH_RAWIO_H
#define VSH_RAWIO_H

#define COMMAND_SIZE 128

extern char *inp;
extern int pt;

void die(const char* s);
void disableRawMode();
void enableRawMode();
char* rawio();

#endif //VSH_RAWIO_H
