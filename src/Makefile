# -*- Makefile -*-

.PHONY: all clean

all: main.o httpMethods.o utils.o
	gcc main.o httpMethods.o utils.o -o htbench

main.o: main.c inputs.h httpMethods.h constants.h
	gcc -c main.c

httpMethods.o: httpMethods.c httpMethods.h inputs.h constants.h utils.h
	gcc -c httpMethods.c

utils.o: utils.c utils.h
	gcc -c utils.c

clean:
	rm -rf *o htbench