#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define INFILE "forkwithfile.c"
#define BUFSIZE 128

int main(int argc, char *argv[]) {
    int fd;
    char buf[BUFSIZE];
    printf("PID: %ld\n", (long)getpid());
    printf("Each process will read in and print out up to 127 characters.\n");
    fflush(stdout);

    pid_t pid = fork();
    if (pid < 0) {
        fprintf(stderr, "Error: fork() failed. %s.\n", strerror(errno));
        return EXIT_FAILURE;
    } else if (pid > 0) {
        // We're in the parent.
        fd = open(INFILE, O_RDONLY);
        if (fd < 0) {
            fprintf(stderr, "Error: Cannot open file '%s'. %s.\n", INFILE, strerror(errno));
            return EXIT_FAILURE;
        }
        read(fd, buf, BUFSIZE - 1); // So we can null terminate and use printf()
        buf[BUFSIZE - 1] = '\0';
        printf("\nParent reads:\n----------------------------------------\n%s\n"
               "----------------------------------------\n", buf);
        fflush(stdout);
        close(fd);

        int status;
        do {
            // Wait for the child to complete and get the status of how it terminated.
            pid_t w = waitpid(pid, &status, WUNTRACED | WCONTINUED);
            if (w == -1) {
                perror("waitpid()"); // Lazy way of fprintf
                exit(EXIT_FAILURE);
            }
            if (WIFEXITED(status)) {
                printf("Child process %ld exited, status=%d.\n", (long)pid, WEXITSTATUS(status));
            } else if (WIFSIGNALED(status)) {
                printf("Child process %ld killed by signal %d.\n", (long)pid, WTERMSIG(status));
            } else if (WIFSTOPPED(status)) {
                printf("Child process %ld stoppped by signal %d.\n", (long)pid, WSTOPSIG(status));
            } else if (WIFCONTINUED(status)) {
                printf("Child process %ld continued.\n", (long)pid);
            } 
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    } else {
        // We're in the child.
        // Let the child sleep for 2 seconds so that parent MUST wait.
        sleep(30);
        fd = open(INFILE, O_RDONLY);
        if (fd < 0) {
            fprintf(stderr, "Error: Cannot open file '%s'. %s.\n", INFILE, strerror(errno));
            return EXIT_FAILURE;
        }
        read(fd, buf, BUFSIZE - 1); // So we can null terminate and use printf()
        buf[BUFSIZE - 1] = '\0';
        printf("\nChild reads:\n----------------------------------------\n%s\n"
               "----------------------------------------\n", buf);
        fflush(stdout);
        close(fd);
    }

    return EXIT_SUCCESS;
}