#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    int pid = fork();

    if (pid < 0) {
        fprintf(stderr, "An error occurred!\n");
    } else if (pid > 0) {
        printf("This is the parent process, and the child process's PID is %d.\n", pid);
    } else {
        printf("This is the child process.\n");
    }

    return EXIT_SUCCESS;
}