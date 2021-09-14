# Vsh
A basic shell made as a part of Operating Systems and Networks course.

## Functionality
- Prompt with username, hostname and directory
- Basic commands : `cd`, `pwd`, `echo`, `ls`, `repeat`
- Ability to run external commands as foreground and background

## Installation
1. Run `make` to build the executable
2. Run `make run` or `./vsh` to start the shell

## File structure
| Functionality |Files |
| -- | -- |
|Prompt| [prompt.c](src/prompt.c) [prompt.h](src/prompt.h)|
|cd, pwd, echo, repeat | [builtins.c](src/builtins.c) [builtins.h](src/builtins.h)|
|ls | [ls.c](src/ls.c) [ls.h](src/ls.h)|
|Input parser| [parse.c](src/parse.c) [parse.h](src/parse.h)|
|External commands|[execute.c](src/execute.c) [execute.h](src/execute.h)|
|Background jobs| [jobs.c](src/jobs.c) [jobs.h](src/jobs.h)|
|Data structure for commands| [args.c](src/args.c) [args.h](src/args.h)|
|Error handling, etc| [utils.c](src/utils.c) [utils.h](src/utils.h)|

