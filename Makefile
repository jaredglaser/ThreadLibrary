# choose your compiler
CC=gcc
#CC=gcc -Wall

threadTest: threadlib.o 
	$(CC) -g main.c threadlib.o -o threadTest 


threadlib.o: threadlib.c threadlib.h
	$(CC) -g -c threadlib.c

clean:
	rm -rf threadlib.o threadTest
