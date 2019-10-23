CC=gcc
DEBUGGER=gdb
PARAMS=-Wall
BIN=wargame

all: wargame.c
	$(CC) $(PARAMS) -o $(BIN) $^

debug: wargame.c
	$(CC) $(PARAMS) -g -o $(BIN) $^
	$(DEBUGGER) ./$(BIN)
.PHONY: clean
clean:
	rm -f *.o $(BIN)
