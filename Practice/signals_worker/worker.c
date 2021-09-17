#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* Volatile variables are always read from memory and not from a register.
   Compiler optimizations are disabled for volatile variables.
   The type sig_atomic_t is an integer that can be accessed as an atomic entity
   even in the presence of asynchronous interrupts.
*/
volatile sig_atomic_t signal_value = 0;

/* Signal handler -- keep it simple, stupid */
void catch_signal(int sig) {
    signal_value = sig;
}

int main() {
    struct sigaction action;
    // First we want to zero out this block of memory.
    memset(&action, 0, sizeof(struct sigaction));
    action.sa_handler = catch_signal;
    action.sa_flags = SA_RESTART; // Resart syscall if possible.

    if (sigaction(SIGINT, &action, NULL) == -1) {
        perror("sigaction");
        return EXIT_FAILURE;
    }
    // Attempting to terminate the process.
    if (sigaction(SIGTERM, &action, NULL) == -1) {
        perror("sigterm");
        return EXIT_FAILURE;
    }

    while (true) {
        if (!signal_value) {
            printf("I am busy working. Do not disturb me!\n");
        } else if (signal_value == SIGINT) {
            printf("ARGH! I told you NOT to disturb me!!!\n");
            signal_value = 0;
        } else if (signal_value == SIGTERM) {
            printf("YOU TRIED TO TERMINATE ME???\n");
            signal_value = 0;
        }
        sleep(1);
    }
    return EXIT_SUCCESS;
}