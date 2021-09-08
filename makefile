main:
	gcc -o vsh src/*.c

debug:
	gcc -g -o vsh src/*.c

run:
	./vsh

