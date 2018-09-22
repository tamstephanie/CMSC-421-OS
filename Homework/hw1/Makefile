CFLAGS = -g -ansi -Wall

leaky.out: hw1_leaky.o
	gcc ${CFLAGS} hw1_leaky.c -o $@

nonleaky.out: hw1_leaky_nonleaky.o
	gcc ${CFLAGS} hw1_leaky_nonleaky.c -o $@

clean:
	rm -f *~* *.o*
