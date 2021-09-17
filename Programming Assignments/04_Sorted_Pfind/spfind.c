/*******************************************************************************
 * Name        : spfind.c
 * Author      : Matthew Oyales & Audie Breslin
 * Date        : 16 June 2021
 * Description : Sorted Permission Find
 * Pledge      : I pledge my honor that I have abided by the Stevens Honor System.
 ******************************************************************************/
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/wait.h>

void print_usage(char *progname) {
    fprintf(stderr, "Usage: %s -d <directory> -p <permissions string> [-h]\n", progname);
}

int main(int argc, char *argv[]) {
    if (argc == 1) {
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    // Declare the pipes that will have to be used.
    int pfind_to_sort[2], sort_to_parent[2];
    pipe(pfind_to_sort);
    pipe(sort_to_parent);

    // pid array of size 2 for the two child processes.
    pid_t pid[2];
    if ((pid[0] = fork()) == 0) {
        close(pfind_to_sort[0]);
        dup2(pfind_to_sort[1], STDOUT_FILENO);

        // Close unrelated
        close(sort_to_parent[0]);
        close(sort_to_parent[1]);

        // Attempt to execute.
        execv("pfind", argv);
    } else if (pid[0] < 0) {
        // In the case that fork returns something < 0
        fprintf(stderr, "Error: fork() failed. %s.\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    if ((pid[1] = fork()) == 0) {
        dup2(pfind_to_sort[0], STDIN_FILENO); // Read output from pfind.
        close(pfind_to_sort[1]);

        close(sort_to_parent[0]);
        dup2(sort_to_parent[1], STDOUT_FILENO);

        execlp("sort", "sort", NULL);

        printf("Error: sort failed.\n");
        exit(EXIT_FAILURE);
    } else if (pid[1] < 0) {
        fprintf(stderr, "Error: fork() failed. %s.\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    // In the parent now.
    // Close out unrelated.
    close(pfind_to_sort[0]);
    close(pfind_to_sort[1]);

    // Only pipe we need is reading from output of sort.
    dup2(sort_to_parent[0], STDIN_FILENO);
    close(sort_to_parent[1]);

    char buf[PATH_MAX];  // Storage for the output          
    int matches = 0;     // Store of permissions that match.
    bool usage = false;  // A flag in case the "usage" message pops up in buf.
    buf[0] = '\0';       // Initial null termination of buf.
    ssize_t lines;       // Result of reading from STDIN_FILENO.
    
    // Go through the lines until we reach EOF (when read returns 0)
    while ((lines = read(STDIN_FILENO, buf, sizeof(buf) - 1))) {
        // At some point, read could fail.
        if (lines < 0) {
            fprintf(stderr, "read() failed.\n");
            return EXIT_FAILURE;
        }
        // At some point, write could fail.
        if (write(STDOUT_FILENO, buf, lines) < 0) {
            fprintf(stderr, "write() failed.\n");
            return EXIT_FAILURE;
        }
        
        // Because we forgot to null terminate end of buf, we got the valgrind errors.
        // Reading a non-terminated string can lead to unexpected behavior.
        buf[lines] = '\0';
        
        // If the usage message got into the buffer (when -h is called in child),
        // Set a flag such that the "total matches" will not print. Break out early.
        if (strstr(buf, "Usage: ./spfind -d <directory> -p <permissions string> [-h]")) {
            usage = true;
            break;
        } 

        // Code to help identify newlines in the buf.
        // Finding one implies another matching entry, so we increment matches.
        for (int i = 0; i < lines; i++) {
            if (buf[i] == '\n') {
                matches++;
            }
        }
    }
    
    // Wait or something
    close(sort_to_parent[0]);

    int status; // Store process saved by waitpid.
    int check1; // Save status of child process 1
    int check2; // Save status of child process 2

    pid_t child;
    pid_t child2;

    // Still need to check if wait fails for either child.
    if ((child = waitpid(pid[0], &status, 0)) < 0) {
        fprintf(stderr, "Error: child wait() failed.\n");
        exit(EXIT_FAILURE);
    }
    check1 = status;

    if ((child2 = waitpid(pid[1], &status, 0)) < 0) {
        fprintf(stderr, "Error: child2 wait() failed.\n");
        exit(EXIT_FAILURE);
    }
    check2 = status;

    // Ensure that both waits are successful before printing.
    if ((WIFEXITED(check1) && WEXITSTATUS(check1) == EXIT_SUCCESS) && (WIFEXITED(check2) && WEXITSTATUS(check2) == EXIT_SUCCESS) && !usage) {
        printf("Total matches: %d\n", matches);
    }
    
    return EXIT_SUCCESS;
}