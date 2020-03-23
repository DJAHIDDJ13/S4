FLAGS=-Wall -g 

all: tp1

tp1.o: tp1.c 
	gcc $(FLAGS) -c tp1.c nrio.c nralloc.c nrarith.c -lm -w 

tp1: tp1.o 
	gcc $(FLAGS) -o tp1 tp1.o nrio.c nralloc.c nrarith.c -lm -w 

.PHONY: clean all
clean: 
	rm -f -rf *.o



