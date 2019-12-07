FLAGS=-Wall -g
LIBS=-lm

all: main.o
	gcc main.o -o main $(FLAGS) $(LIBS)

main.o: main.c
	gcc -c main.c $(FLAGS)

PHONY: clean
clean:
	rm -f *.o main
