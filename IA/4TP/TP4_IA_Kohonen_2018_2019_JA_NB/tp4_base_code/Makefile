CC=gcc
LIBS=-lm -lglut  -lGL -lGLU
FLAGS=-g -Wall $(pkg-config --cflags glu)
BIN=main

all: $(BIN)

$(BIN): base_opengl.o ppm.o kohonen.o
	$(CC) $(FLAGS) $^ -o $(BIN) $(LIBS)

ppm.o: ppm.c ppm.h
	$(CC) $(FLAGS) -c $<

base_opengl.o: base_opengl.c base_opengl.h
	$(CC) $(FLAGS) -c $<

kohonen.o: kohonen.c kohonen.h
	$(CC) $(FLAGS) -c $<

base_opengl.h:
	touch base_opengl.h

ppm.h: 
	touch ppm.h

kohonen.h:
	touch kohonen.h

clean :
	rm -f $(BIN) *.o

.PHONY: clean all
