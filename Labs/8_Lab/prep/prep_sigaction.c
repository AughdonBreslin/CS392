#include <errno.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

volatile sig_atomic_t t = 0;

void siguser1_handler(int sig) {
    /* sig - the signal number passed in. */
    char *msg;
    if (sig == SIGUSR1) {
        msg = "SIGUSR1 has been caught!\n";
        write(STDOUT_FILENO, msg, 25);
    } else {
        msg = "Something went wrong! The caught signal was not SIGUSR1!\n";
        write(STDOUT_FILENO, msg, 57);
        return;
    }

    msg = "Time since last SIGUSR1: ";

    write(STDOUT_FILENO, msg, 25);

    int tmp = t;
    int digits = 1;
    int tmp2;
    while (tmp >= 10) {
        tmp /= 10;
        digits += 1;
    }
    for (int i = 0; i < digits; i++) {
        tmp = t;
        while (tmp >= 10) {
            tmp /= 10;
        }
        tmp += 48;
        write(STDOUT_FILENO, (char *)&tmp, 1);

        tmp2 = 1;
        for (int j = digits - i-1; j > 0; j--) {
            tmp2 *= 10;
        }
        t %= tmp2;
    }
    write (STDOUT_FILENO, "\n", 1);
    t = 0;
}

int main() {
    struct sigaction sa;
    memset(&sa, 0, sizeof(struct sigaction));

    sa.sa_handler = siguser1_handler;
    sigemptyset(&sa.sa_mask);

    /* If a blocking system call was interrupted (think "read()"), the system
     * call is restarted. If it fails to restart, then the system call fails
     * and errno is set to EINTR. */
    sa.sa_flags = SA_RESTART;

    if (sigaction(SIGUSR1, &sa, NULL) < 0) {
        fprintf(stderr, "Error: Could not register signal handler. %s\n.", strerror(errno));
        return EXIT_FAILURE;
    }

    while (true) {
        sleep(1);
        t++;
    }
}