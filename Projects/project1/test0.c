#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include "fifo.c"

#define __NR_enqueue421 351
#define __NR_dequeue421 352
#define __NR_peekLen421 353
#define __NR_queueLen421 354
#define __NR_clearQueue421 355
#define MAX_LEN 512

long enqueue421_syscall(void) {
  return sys_enqueue421(__NR_enqueue421, (void*)0x0000, MAX_LEN);
}

long dequeue421_syscall(void) {
  return sys_dequeue421(__NR_dequeue421, MAX_LEN);
}

long peekLen421_syscall(void) {
  return sys_peekLen421();
}

long queueLen421_syscall(void) {
  return sys_queueLen421(__NR_queueLen421);
}

long clearQueue421_syscall(void) {
  return sys_clearQueue421(__NR_clearQueue421);
}

int main(int argc, char* argv[]) {
  enqueue421_syscall();
  dequeue421_syscall();
  peekLen421_syscall();
  queueLen421_syscall();
  clearQueue421_syscall();

  return 0;
}
