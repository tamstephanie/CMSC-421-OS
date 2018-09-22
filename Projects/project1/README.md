# Project 1
**Due Date: 2017 Nov. 6, 11:59PM (extension to Nov. 8)**
Author: Stephanie Tam
Email: ru14618@umbc.edu

## Introduction/Objectives
Create a new custom kernel that contains a few new system calls. Before beginning, create a new working copy of the kernel source tree, into /usr/src/project1.

### Objectives:
1. Becomes familiar w/ procedure of adding a system call to x86 (32-bit) version of Linux kernel.
2. Practice dynamic memory allocation in kernel space.
3. Practice adequate testing of kernel-level code.
4. Become familiar w/ manually calling Linux system calls from userland.

## Approach
I decided to follow Professor Park's suggested strategy for this project. I couldn't follow it exactly, due to some initial confusion about the Part II testing, which I partially skipped. I chose this approach, not only because the professor suggested that we follow this, but also because it did make the most sense in terms of working on it incrementally and being able to fully understand what the project's purpose is.

Below is a more detailed description of my approach in each part/phase:
**Part 0:**
I decided to follow the suggested Project 1 strategy from Professor Park as best as I could because it seemed like the best way to approach the project. I did the "Hello World" system call exercise to familiarize myself w/ creating a system call, which I found quite helpful.

**Part I:**
I moved onto tackling the actual project. I created the necessary directories and files (fifo/fifo.c, fifo/Makefile), then I added the basic skeleton of the syscall functions with the required "#includes". Upon compiling the first time with the basics, there were no issues.

**Part II:**
I added print statements for the length and sizes. Passing in the dummy parameters were pretty simple, though I wasn't completely sure I had been doing it right. I decided to continue on with the project and implement the semantics of each function, working incrementally.

**Part III:**
When I first began, I looked at the resources provided in the project description, which are also listed below in "Resources". As I worked, these resources, in addition to a couple of other resources, were helpful in figuring out what functions I could utilize and how to implement them in my program.

I worked backwards, starting with the simpler calls of sys_clearQueue421(), sys_queueLen421(), and sys_peekLen421(). I tested these three to make sure that they at least ran. Next, I tackled sys_enqueue421() and sys_dequeue421(). The basic tests showed that the functions seemed to be working, but upon more in-depth testing (test_fifo.c), I had forgotten to copy the message into the new item in enqueue, so I was getting an error upon trying to grab it in dequeue.

In terms of writing fifo.c, I dealt with locking last. I didn't understand it fully, but with some explanation from friends, and then looking at the documentation of <linux/semaphore.h>, I understand it now. It's quite simple now to determine which items could be accessed or changed out of order and appropriately lock them so that neither race conditions nor starvation occur.

Lastly, I tested my program against Prof. Park's test_fifo.c file and my own test.c file. For my own test file, I made it more robust to handle both valid and invalid input. I had a separate call for the valid and invalid tests so that it is easier to see what works and what doesn't. Everything in test.c and fifo.c should be commented or contain print statements detailing intentions and what happened upon running the tests.

**Resources:**
1. <[linux/list.h](http://elixir.free-electrons.com/linux/latest/source/include/linux/list.h)>
2. [Kernel Korner: System Calls](https://www.csee.umbc.edu/courses/undergraduate/CMSC421/fall02/burt/projects/LinuxJournal_SystemCalls.html)
3. [Linux Kernel API](https://www.kernel.org/doc/htmldocs/kernel-api/)
4. [Unreliable Guide to Locking](https://www.kernel.org/doc/htmldocs/kernel-locking/index.html)
5. [Linux Kernel Linked List Explained](https://isis.poly.edu/kulesh/stuff/src/klist/)
6. [Kernel Data Structures](https://notes.shichao.io/lkd/ch6/)
7. <[linux/semaphore.h](http://elixir.free-electrons.com/linux/latest/source/include/linux/semaphore.h)>

**NOTE: THE FILES ARE ARRANGED LIKE THE KERNEL (driver > src > files changed and added in kernel)**

## Driver Programs
### How to Use The Programs
**test.c**
- What It Tests
  - Valid input
    - Single enqueue and dequeue
    - Multiple enqueues and dequeues (varying size and set size)
    - Expected errors with syscalls
  - Invalid input
    - sys_enqueue421() errors
    - sys_dequeue421() errors
    - sys_peekLen421() errors
    - _NOTE: Park specifically said that we don't need to test ENOMEM error cases_

- How to Test It
  - Steps:
    1. make test (also works with just 'make')
    2. valid input:   ./test 0
       invalid input: ./test -1

**test_fifo.c** _(Note: This is Professor Park's provided test file)_
- What It Tests
  - different phases of program development

- How to Test It
  - Steps:
    1. make test_fifo
    2. ./test_fifo <phase#:1-4> _(Example: ./test_fifo 3)_
