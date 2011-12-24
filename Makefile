CC = gcc -lpthread

CFLAGS = -g

all: mergetest

mergetest: mergetest.o mergesort.o

mergetest.o: mergetest.c mergesort.h

mergesort.o: mergesort.h mergesort.c