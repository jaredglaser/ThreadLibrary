# Makefile for UD CISC user-level thread library

CC = gcc
CFLAGS = -g

LIBOBJS = threadlib.o 

TSTOBJS = test.o 

# specify the executable 

EXECS = test

# specify the source files

LIBSRCS = threadlib.c

TSTSRCS = test.c

# ar creates the static thread library

threadlib.a: ${LIBOBJS} Makefile
	ar rcs threadlib.a ${LIBOBJS}

# here, we specify how each file should be compiled, what
# files they depend on, etc.

threadlib.o: threadlib.c threadlib.h Makefile
	${CC} ${CFLAGS} -c threadlib.c

test.o: test.c ud_thread.h Makefile
	${CC} ${CFLAGS} -c test.c

test: test.o threadlib.a Makefile
	${CC} ${CFLAGS} test.o threadlib.a -o test

clean:
	rm -f threadlib.a ${EXECS} ${LIBOBJS} ${TSTOBJS} 
