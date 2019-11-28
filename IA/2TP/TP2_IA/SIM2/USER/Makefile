all: fuzzy_test.o fuzzy_infer.o fuzzy_utils.o
	gcc -g fuzzy_test.o fuzzy_infer.o fuzzy_utils.o -o fuzzy_test -Wall

fuzzy_test.o: fuzzy_test.c fuzzy_infer.c fuzzy_utils.c fuzzy_infer.h
	gcc -g -c -Wall $<

fuzzy_infer.o: fuzzy_infer.c fuzzy_utils.c fuzzy_infer.h
	gcc -g -c -Wall $<

fuzzy_utils.o: fuzzy_utils.c fuzzy_infer.h
	gcc -g -c -Wall $<

.PHONY: clean
clean:
	rm -f *.o fuzzy_test
