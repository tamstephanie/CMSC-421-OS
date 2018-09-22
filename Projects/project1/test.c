#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <linux/kernel.h>
#include <sys/syscall.h>
#include <string.h>
#include <errno.h>

#define __NR_enqueue421 351
#define __NR_dequeue421 352
#define __NR_peekLen421 353
#define __NR_queueLen421 354
#define __NR_clearQueue421 355

#define MSGS 8

/* FUNCTIONS COPIED FROM PARK'S TEST_FIFO.C FOR EASE OF PRINTING INFORMATION */
long enqueue421(void *data, long len) {
    long val;

    printf("  enqueue421(data=%p (\"%.*s\"), len=%d): returned ", data, len, data, len);
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

    printf("  dequeue421(data=%p, len=%d): returned ", data, len);
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

    printf("  peekLen421(): returned ");
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

    printf("  queueLen421(): returned ");
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

    printf("  clearQueue421(): returned ");
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


/* BEGIN ROBUST TESTING */
int main(int argc, char *argv[]) {
  int test_ver;

  char wbuf[MSGS][512];
  char rbuf[513];
  long len;
  int i;

  /* Make sure correct number of arguments inserted */
  if(argc != 2) {
    fprintf(stderr, "syntax: %s <valid/invalid: 0/-1>\n", argv[0]);
    exit(1);
  }

  printf("BEGIN TEST FILE\n");
  test_ver = atoi(argv[1]); /* Determine whether to run valid input test or invalid input test */

  if(test_ver == 0) {
    printf("----------- VALID INPUT TEST -----------\n\n");

    /* Add one item to queue */
    printf("Round 1: Single enqueue and dequeue\n");
    enqueue421("CMSC 421: Operating SystemsXXX", 27);
    queueLen421();
    peekLen421();
    memset(rbuf, 0, sizeof(rbuf));
    dequeue421(rbuf, sizeof(rbuf) - 1);
    printf("  Expected \"CMSC 421: Operating Systems\"; Got \"%s\"", rbuf);
    if (strcmp(rbuf, "CMSC 421: Operating Systems")) {
       printf("ERROR!\n");
    }
    else {
       printf("\n");
    }
    queueLen421();
    clearQueue421();   
    printf("  Should see error:");
    peekLen421();

    printf("\nRound 2: Multiple enqueues and dequeues w/ different message sizes\n");
    for(i = 0; i < MSGS; i++) {
      sprintf(wbuf[i], "Ltr A%.*s", i, "BCDEFGHIJKL");
      enqueue421(wbuf[i], strlen(wbuf[i]));
    }
    queueLen421();
    for(i = 0; i < MSGS; i++) {
      printf("  Should see len=%d", strlen(wbuf[i]));
      len = peekLen421();
      if(len != strlen(wbuf[i])) {
	printf("ERROR!\n");
      }
      memset(rbuf, 0, sizeof(rbuf));
      dequeue421(rbuf, 512);
      printf("  Expected \"%s\"; Got \"%s\"", wbuf[i], rbuf);
      if (strcmp(rbuf, wbuf[i])) {
        printf("ERROR!\n");
      }
      else {
        printf("\n");
      }
      queueLen421();
    }
    printf("  Should see error:");
    peekLen421();

    for(i = 0; i < 5; i++) {
      enqueue421("CMSC 421 Operating Systems Project 1", 36);
    }
    printf("queuelen should be 5:");
    queueLen421();
    memset(rbuf, 0, sizeof(rbuf));
    dequeue421(rbuf, 512);
    clearQueue421();
    printf("queuelen should be 0:");
    queueLen421();
    printf("dequeue should fail:");
    dequeue421(rbuf, 512);

    printf("\nRound 3: Multiple enqueues and dequeues switching back and forth\n");
    enqueue421("CMSC 421 Operating Systems", 27);
    enqueue421("Professor: John Park", 20);
    enqueue421("TA: Leslie McAdoo", 17);
    printf("queuelen should be 3:");
    queueLen421();
    peekLen421();
    memset(rbuf, 0, sizeof(rbuf));
    dequeue421(rbuf, sizeof(rbuf) - 1);
    memset(rbuf, 0, sizeof(rbuf));
    dequeue421(rbuf, sizeof(rbuf) - 1);
    printf("  Expected \"Professor: John Park\"; Got \"%s\"", rbuf);
    if (strcmp(rbuf, "Professor: John Park")) {
       printf("ERROR!\n");
    }
    else {
       printf("\n");
    }
    printf("queuelen should be 1:");
    queueLen421();
    peekLen421();
    enqueue421("Student: Stephanie Tam", 22);
    enqueue421("Student: Veronica Clements", 26);
    printf("queuelen should be 3:");
    queueLen421();
    clearQueue421();
    printf("queuelen should be 0:");
    queueLen421();

    printf("\nTests complete.\n");
    printf("\n--------- END VALID INPUT TEST ---------\n");
  }
  else if(test_ver == -1) {
    printf("---------- INVALID INPUT TEST ----------\n\n");
    
    printf("sys_enqueue421(): testing correct handling of errors\n");
    enqueue421("len < 0 error", -1);
    enqueue421("len > 512 error", 791);
    enqueue421(NULL, 512);
    
    printf("\nsys_dequeue421(): testing correct handling of errors\n");
    memset(rbuf, 0, sizeof(rbuf));
    dequeue421(rbuf, -1);
    
    enqueue421("valid placeholder", 17);
    memset(rbuf, 0, sizeof(rbuf));
    dequeue421(rbuf, 5);
    memset(rbuf, 0, sizeof(rbuf));
    printf("  dequeue should succeed:");
    dequeue421(rbuf, 512);
    memset(rbuf, 0, sizeof(rbuf));
    printf("  queuelen should be 0-->dequeue should fail:");
    dequeue421(rbuf, 512);

    printf("\nsys_peekLen421(): testing correct handling of errors\n");
    /* This section before peekLen421() should get queuelen = 0 */

    printf("  queuelen should be 0:");
    queueLen421();
    printf("  peeklen should fail:");
    peekLen421();

    printf("\n-------- END INVALID INPUT TEST --------\n");
  }
  else {
    fprintf(stderr, "Invalid argument given. Should be either 0 (valid) or -1 (invalid).\n");
    exit(1);
  }

  printf("\nEND TEST FILE\n");
  return 0;
}
