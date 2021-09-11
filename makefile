main:
	gcc -o vsh src/*.c

debug:
	gcc -g -o vsh src/*.c -Wall -Wextra

run:
	./vsh
val:
	valgrind --leak-check=full ./vsh

