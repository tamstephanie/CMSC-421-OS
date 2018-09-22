CFLAGS= -g -Wall -ansi

test: test.o
	gcc ${CFLAGS} test.c -o test

test_fifo: test_fifo.o
	gcc ${CFLAGS} test_fifo.c -o test_fifo

clean:
	rm -f *~
	rm -f *.o*
