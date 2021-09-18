//
// Created by vegi on 18/09/21.
//

#ifndef VSH_RAWIO_H
#define VSH_RAWIO_H

#define COMMAND_SIZE 128

void die(const char* s);
void disableRawMode();
void enableRawMode();
char* rawio();

#endif //VSH_RAWIO_H
