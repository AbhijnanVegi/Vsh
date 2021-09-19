#include <stdio.h>
#include <termios.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>

#include "history.h"
#include "rawio.h"
#include "prompt.h"

void die(const char *s) {
    perror(s);
    exit(1);
}

struct termios orig_termios;

void disableRawMode() {
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)
        die("tcsetattr");
}

/**
 * Enable row mode for the terminal
 * The ECHO feature causes each key you type to be printed to the terminal, so you can see what you’re typing.
 * Terminal attributes can be read into a termios struct by tcgetattr().
 * After modifying them, you can then apply them to the terminal using tcsetattr().
 * The TCSAFLUSH argument specifies when to apply the change: in this case, it waits for all pending output to be written to the terminal, and also discards any input that hasn’t been read.
 * The c_lflag field is for “local flags”
 */
void enableRawMode() {
    if (tcgetattr(STDIN_FILENO, &orig_termios) == -1) die("tcgetattr");
    atexit(disableRawMode);
    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ICANON | ECHO);
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) die("tcsetattr");
}

/**
 * stdout and stdin are buffered we disable buffering on that
 * After entering in raw mode we read characters one by one
 * Up arrow keys and down arrow keys are represented by 3 byte escape codes
 * starting with ascii number 27 i.e. ESC key
 * This way we interpret arrow keys
 * Tabs are usually handled by the term, but here we are simulating tabs for the sake of simplicity
 * Backspace move the cursor one control character to the left
 * @return
 */

char *inp;
int pt;

char* rawio() {
    char c;
    inp = malloc(sizeof(char) * COMMAND_SIZE);
    pt = 0;
    int history = -1;
    ArgList* h = read_history();
        setbuf(stdout, NULL);
        enableRawMode();
        memset(inp, '\0', 100);
        while (read(STDIN_FILENO, &c, 1) == 1) {
            if (iscntrl(c)) {
                if (c == 10) {
                    printf("%c", c);
                    break;
                }
                else if (c == 27) {
                    char buf[3];
                    buf[2] = 0;
                    if (read(STDIN_FILENO, buf, 2) == 2) { // length of escape code
                       if (strcmp(buf,"[A") == 0)
                       {
                            printf("\r");
                            prompt();
                            for (int i = 0; i < pt; i++)
                                printf(" ");
                            printf("\r");
                            prompt();
                            history = history + 1 >= h->size ? h->size - 1 : history + 1;
                            pt = strlen(h->args[history]);
                            if (h->args[history][pt-1] == '\n')
                            {
                                h->args[history][pt-1] = '\0';
                                pt--;
                            }
                            printf("%s", h->args[history]);
                            free(inp);
                            inp = strdup(h->args[history]);
                       }
                    }
                } else if (c == 127) { // backspace
                    if (pt > 0) {
                        if (inp[pt-1] == 9) {
                            for (int i = 0; i < 7; i++) {
                                printf("\b");
                            }
                        }
                        inp[--pt] = '\0';
                        printf("\b \b");
                    }
                } else if (c == 9) { // TAB character
                    inp[pt++] = c;
                    for (int i = 0; i < 8; i++) { // TABS should be 8 spaces
                        printf(" ");
                    }
                } else if (c == 4) {
                    exit(0);
                } else {
                    printf("%d\n", c);
                }
            } else {
                inp[pt++] = c;
                printf("%c", c);
            }
        }
        disableRawMode();

    return inp;
}


