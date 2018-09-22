/* #define _GNU_SOURCE */

#include <stdio.h>
#include <unistd.h>
#include <linux/kernel.h>
#include <sys/syscall.h>
#include <string.h>
#include <errno.h>

#define __NR_enqueue421		351
#define __NR_dequeue421		352
#define __NR_peekLen421		353
#define __NR_queueLen421	354
#define __NR_clearQueue421	355

#define RND2_RNDS	5


long enqueue421(void *data, long len) {
    long val;

    printf("enqueue421(data=%p (\"%.*s\"), len=%d): returned ", data,
	   len, data, len);
    fflush(stdout);
    val = syscall(__NR_enqueue421, data, len);
    printf("%d", val);
    fflush(stdout);
    if (val < 0)
	perror("; ERR:");
    else
	printf("\n");
    return val;
}

long dequeue421(void *data, long len) {
    long val;

    printf("dequeue421(data=%p, len=%d): returned ", data, len);
    fflush(stdout);
    val = syscall(__NR_dequeue421, data, len);
    printf("%d", val);
    fflush(stdout);
    if (val < 0)
	perror("; ERR:");
    else
	printf("\n");
    return val;
}

long peekLen421() {
    long val;

    printf("peekLen421(): returned ");
    fflush(stdout);
    val = syscall(__NR_peekLen421);
    printf("%d", val);
    fflush(stdout);
    if (val < 0)
	perror("; ERR:");
    else
	printf("\n");
    return val;
}

long queueLen421() {
    long val;

    printf("queueLen421(): returned ");
    fflush(stdout);
    val = syscall(__NR_queueLen421);
    printf("%d", val);
    fflush(stdout);
    if (val < 0)
	perror("; ERR:");
    else
	printf("\n");
    return val;
}

long clearQueue421() {
    long val;

    printf("clearQueue421(): returned ");
    fflush(stdout);
    val = syscall(__NR_clearQueue421);
    printf("%d", val);
    fflush(stdout);
    if (val < 0)
	perror("; ERR:");
    else
	printf("\n");
    return val;
}

int main(int argc, char **argv) {
    int phase;
    char wbuf[RND2_RNDS][512];
    char rbuf[513];
    long len;
    int i;

    if (argc != 2) {
	fprintf(stderr, "syntax: %s <phase#:1-4>\n", argv[0]);
	exit(1);

    }

    phase = atoi(argv[1]);

    if (phase == 1) {
	     printf("Phase I: Simple calls\n");
	     printf("Ignore error msgs from this program--use \"dmesg\" to see kernel printfs.\n");
    }
    else if (phase == 2) {
	     printf("Phase II: Simple calls w/parameter validation\n");
	     printf("Ignore error msgs from this program--use \"dmesg\" to see kernel printfs.\n");
	     printf("Verify that kernel funcs received args passed in.\n");
    }
    else if (phase == 3) {
	     printf("Phase III: Actual FIFO421 functionality validation\n");
    }
    enqueue421("Hello, worldXXXXX", 12);
    queueLen421();
    peekLen421();
    memset(rbuf, 0, sizeof(rbuf));
    dequeue421(rbuf, sizeof(rbuf) - 1);
    printf("  Expected \"Hello, world\"; Got \"%s\"", rbuf);
    if (strcmp(rbuf, "Hello, world")) {
	     printf("ERROR!\n");
    } else {
	     printf("\n");
    }
    queueLen421();
    clearQueue421();

    printf("Should see error: ");
    peekLen421();

    if (phase == 4) {
	     printf("Round 2: Multiple enqueues, multiple dequeues\n");
	     for (i = 0; i < RND2_RNDS; i++) {
	        /* Each msg will be a different size: */
	        sprintf(wbuf[i], "Msg 0%.*s", i, "1234567890");
	        enqueue421(wbuf[i], strlen(wbuf[i]));
	     }
	     queueLen421();
	     for (i = 0; i < RND2_RNDS; i++) {
	        printf("Should see len=%d: ", strlen(wbuf[i]));
	        len = peekLen421();
	        if (len != strlen(wbuf[i])) {
		          printf("ERROR!\n");
	        }
	        memset(rbuf, 0, sizeof(rbuf));
	        dequeue421(rbuf, 512);
	        printf("  Expected \"%s\"; Got \"%s\"", wbuf[i], rbuf);
	        if (strcmp(rbuf, wbuf[i])) {
		          printf("ERROR!\n");
	        } else {
		          printf("\n");
	        }
	        queueLen421();
	     }

	for (i = 0; i < 3; i++) {
	    enqueue421("Hello, world", 12);
	}
	printf("queuelen should be 3: ");
	queueLen421();
	clearQueue421();
	printf("queuelen should be 0: ");
	queueLen421();
	printf("dequeue should fail: ");
	dequeue421(rbuf, 512);
	printf("Tests complete\n");
    }
    return 0;
}
