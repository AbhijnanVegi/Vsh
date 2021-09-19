DIR=src


all:
	@echo "Building..."
	gcc $(DIR)/*.c -o vsh
	@echo "vsh successfully built"