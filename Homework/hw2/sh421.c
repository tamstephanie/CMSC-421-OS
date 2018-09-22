#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>

int main() {
    pid_t pid;

    /*fork a child process*/
    pid = fork();

    if(pid < 0) {   /*error occurred*/
        fprintf(stderr, "Fork Failed");
        return 1;
    }
    else if(pid == 0) {   /*child process*/
        execvp("ls", char *args[NARGS+1]);
    }
    else {   /*parent process*/
        /*parent will wait for child to complete*/
        wait(NULL);
        printf("Child Complete\n");
    }

    return 0;
}
