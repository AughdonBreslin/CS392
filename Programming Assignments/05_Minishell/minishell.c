/*******************************************************************************
 * Name        : minishell.c
 * Author      : Matthew Oyales & Audie Breslin
 * Date        : 23 June 2021
 * Description : A basic shell program in C with some built-in commands.
 * Pledge      : I pledge my honor that I have abided by the Stevens Honor System.
 ******************************************************************************/
#include <errno.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <limits.h>
#include <string.h>
#include <pwd.h>

#define BRIGHTBLUE "\x1b[34;1m"
#define DEFAULT    "\x1b[0m"
#define BUFSIZE     128

volatile sig_atomic_t signal_value = 0;

void catch_signal(int sig) {
    signal_value = sig;
}

/* HEY MATT!! Saw the sunrise which ended up just being clouds and fog,
 but I've been getting a lot of work done with the start of minishell!
 Hope your CS 581 assignment is going well and lets get this bread. */ 

/**
 * MORNING AUDIE!! I tested this further, and this is what I have to say:
 * - Found a segfault for cd - fixed that with logic rearranging
 * - Formatted the interrupr for sleep
 * - Cleaned up code in some spots, added comments where needed.
 * 
 * This was a lot of hard work, but I think this should be the one.
 */


/**
 * cd
 * 
 * Handle the execution of the "cd" command in the minishell.
 * Takes in the list of arguments and the length of that list.
 * Error out if too many arguments or if directory is invalid.
 */
int cd(char *lst[], int length) {
    // Most likely don't need, but good to have for safety.
    if (length > 2) {
        fprintf(stderr, "Too many arguments for cd.\n");
        return EXIT_FAILURE;
    }

    // Trying to get to home directory
    if (length == 1 || ((length == 2) && (strcmp(lst[1], "~") == 0))) {
        // man getpwuid and getuid
        // Kevin had us clean this up to simplify from what it was before.
        struct passwd *home;
        if((home = getpwuid(getuid())) == NULL) {
            fprintf(stderr, "Error: Cannot get passwd entry. %s.\n",
                    strerror(errno));
            return EXIT_FAILURE;
        }
        if(chdir(home->pw_dir) != 0) {
            fprintf(stderr, "Error: Cannot change directory to '%s'. %s.\n",
                    home->pw_dir, strerror(errno));
            return EXIT_FAILURE;
        }
    } else {
        // Changing directory
        if (chdir(lst[1]) != 0) {
            fprintf(stderr, "Error: Cannot change directory to '%s'. %s.\n",
                        lst[1], strerror(errno));
            return EXIT_FAILURE;
        }
    }
    return EXIT_SUCCESS;
}

int main() {
    // Create a signal handler
    struct sigaction action;

    // First we want to zero out this block of memory.
    memset(&action, 0, sizeof(struct sigaction));
    action.sa_handler = catch_signal;

    if (sigaction(SIGINT, &action, NULL) == -1) {
        perror("sigaction");
        return EXIT_FAILURE;
    }

    // Start the shell up!
    while (true) {  
        // Print the current working directory.
        // man 2 cwd
        char cwd[PATH_MAX];
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("%s[", DEFAULT);
            printf("%s%s", BRIGHTBLUE, cwd);
            printf("%s]$ ", DEFAULT);
        } else { // CWD Error
            fprintf(stderr, "Error: Cannot get current working directory. %s.\n",
                    strerror(errno));
            return EXIT_FAILURE;
        }

        /******************
         * Read in a line *
         ******************/
        char buf[BUFSIZE];
        fflush(stdout);
        int bytes_read = read(STDIN_FILENO, buf, BUFSIZE);
        if (bytes_read < 0) {
            // If you Ctrl+C with nothing else
            if (errno == EINTR) {
                printf("\n");
                continue;
            }
            fprintf(stderr, "Error: Failed to read from stdin. %s.\n",
                    strerror(errno));
            return EXIT_FAILURE;
        }
        // Enter key case.
        if (buf[0] == '\n') {
            continue;
        }
        buf[bytes_read - 1] = '\0';
        //If line is empty

        /**********************
         * Interpret the line *
         **********************/
        char *arg = strtok(buf, " ");
        char *list[PATH_MAX];
        int i = 0;
        while (arg) {
            list[i] = arg;
            arg = strtok(NULL, " ");
            i++;
        }

        /*
         * You will notice that we handle null-terminating the list after the check
         * for the "cd" command. This is because the function was taking in the list
         * with a null-terminated index, which would cause a segfault if you attempt
         * to run "cd" with no other arguments.
         * 
         * Therefore, we should instead see if the command is cd, and continue in the
         * minishell with a fresh list.
         * 
         * We also don't need to null-terminate the list if we are just going to exit,
         * so I moved that bit of code up too. That way, there is one less level of
         * nesting of if-elses.
         * 
         * Hope this explanation makes sense. The only time you want to null-terminate
         * is when you want to attempt execvp().
         */
        if(strcmp(list[0], "cd") == 0) {
            cd(list, i);
            continue;
        } else if (strcmp(list[0], "exit") == 0) {
            return EXIT_SUCCESS;
        } 

        list[i] = NULL; // Null-terminate argument.
        i++;            // increment size once more.

        /********************
         * Execute the line *
         ********************/
        // Non-Built-Ins
        // reference forkexec.c
        pid_t pid;
        if((pid = fork()) < 0) {
            fprintf(stderr, "Error: fork failed. %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        } else if (pid > 0) {
            // We're in the parent
            int status = 0;
            // Wait for the child to complete
            if (waitpid(pid, &status, 0) < 0) {
                // The sleep interrupt case (with proper formatting!)
                if (errno == EINTR) {
                    printf("\n");
                    continue;
                }
                fprintf(stderr, "Error: waitpid() failed. %s.\n", strerror(errno));
                return EXIT_FAILURE;
            }
        } else {
            // We're in the child
            // list[0] is command, rest are arguments
            if (execvp(list[0], list) == -1) {
                fprintf(stderr, "Error: execvp() failed. %s. \n", strerror(errno));
                exit(EXIT_FAILURE);
            }
        }
    }

    return EXIT_SUCCESS;
}