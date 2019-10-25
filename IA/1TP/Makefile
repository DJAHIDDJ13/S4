CC=gcc
DEBUGGER=gdb
PARAMS=-Wall
BIN=wargame
DOC=doc

all: wargame.c
	$(CC) $(PARAMS) -o $(BIN) $^

debug: wargame.c
	$(CC) $(PARAMS) -g -o $(BIN) $^
	$(DEBUGGER) ./$(BIN)

doc: $(DOC)/rapport.tex genplots
	cd $(DOC) && pdflatex rapport.tex # compile pdf

genplots: plot.py output.csv
	python3 $^ # generate the plots

.PHONY: clean
clean:
	rm -f *.o $(BIN)
