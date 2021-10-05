cmake_minimum_required(VERSION 3.20)
project(Vsh C)

set(CMAKE_C_STANDARD 99)

add_executable(
        vsh
        src/main.c
        src/prompt.c
        src/ls.c
        src/jobs.c
        src/args.c
        src/builtins.c
        src/execute.c
        src/utils.c
        src/history.c
        src/parse.c
        src/rawio.c
        src/baywatch.c
)