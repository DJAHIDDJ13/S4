FLAGS=-Wall -g
LIBS=-lm
DOC=doc

all: perceptron.o
	gcc perceptron.o -o perceptron $(FLAGS) $(LIBS)

perceptron.o: perceptron.c
	gcc -c perceptron.c $(FLAGS)

doc: $(DOC)/rapport.pdf

$(DOC)/rapport.pdf: $(DOC)/rapport.tex
	cd $(DOC) && pdflatex rapport.tex # compile pdf

PHONY: clean doc
clean:
	rm -f *.o perceptron
