#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "semaphore.h"

#define SCALE_FACTOR 1000000

/**
* Returns a random integer in the range [low, high].
*/
int random_int_in_range(int low, int high) {
    return low + rand() % (high - low + 1);
}

/**
* Causes the process to sleep for a random amount of time.
*/
void random_sleep() {
    long pid = (long)getpid();
    srand(time(NULL) ^ pid);
    useconds_t sleep_time =
        (useconds_t)random_int_in_range(0 * SCALE_FACTOR, 0.75 * SCALE_FACTOR);
    usleep(sleep_time);
}

int main() {
    key_t key;
    int shmid = -1;
    int retval = EXIT_SUCCESS;
    char *shbuf = (char *)-1;
    int sem = -1;

    // Key is the jey that can be passed when creating a semaphore, or when
    // creating shared memory.
    if ((key = ftok("./", 'k')) == -1) {
        fprintf(stderr, "Error: Failed to create a key. %s.\n", strerror(errno));
        return EXIT_FAILURE;
    }

    // Uniquely identifiable token with which to create a chunk of shared memory.
    // Created shared memory that is 1024 bytes large, with permissions rw-rw-rw-
    if ((shmid = shmget(key, sizeof(char) * 1024, IPC_CREAT | 0666)) == -1) {
        fprintf(stderr, "Error: Failed to create shared memory. %s.\n", strerror(errno));
        retval = EXIT_FAILURE;
        goto EXIT;
    }
    
    // Shared memory is attached to program, and reference stored in shbuf.
    // shmat() returns void *, so we should cast to (char *).
    if ((shbuf = (char *)shmat(shmid, NULL, 0)) == (char *)-1) {
        fprintf(stderr, "Error: Failed to attach shared memory. %s.\n", strerror(errno));
        retval = EXIT_FAILURE;
        goto EXIT;
    }

    if ((sem = create_semaphore(key)) == -1) {
        retval = EXIT_FAILURE;
        goto EXIT;
    }

    // Give the semaphor a single permission.
    // One process allowed in the critical section at once.
    if (set_sem_value(sem, 1) == EXIT_FAILURE) {
        retval = EXIT_FAILURE;
        goto EXIT;
    }

    if (fork() == 0) {
        // first child
        random_sleep();
        if (P(sem) == EXIT_FAILURE) {
            fprintf(stderr, "Error: Failed to acquire semaphore in first child.\n");
            return EXIT_FAILURE;
        }
        strcpy(shbuf, "This is a message from the FIRST child!");
        if (V(sem) == EXIT_FAILURE) {
            fprintf(stderr, "Error: Failed to release semaphore in first child.\n");
            return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
    }
    if (fork() == 0) {
        // second child
        random_sleep();
        if (P(sem) == EXIT_FAILURE) {
            fprintf(stderr, "Error: Failed to acquire semaphore in second child.\n");
            return EXIT_FAILURE;
        }
        strcpy(shbuf, "This is a message from the SECOND child!");
        if (V(sem) == EXIT_FAILURE) {
            fprintf(stderr, "Error: Failed to release semaphore in second child.\n");
            return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
    }

    wait(NULL);
    wait(NULL);

    printf("Message in buffer: '%s'.\n", shbuf);

EXIT:
    // Parent process cleanup.
    // First delete the semaphore.
    if (sem != -1 && del_sem_value(sem) == EXIT_FAILURE) {
        printf("failed to delete semaphore\n");
        retval = EXIT_FAILURE;
    }
    // Detach shared memory buffer.
    if (shbuf != (char *)-1 && shmdt((void *)shbuf) == -1) {
        fprintf(stderr, "Error: Failed to detach shared memory. %s.\n", strerror(errno));
        retval = EXIT_FAILURE;
    }
    // Delete shared memory buffer.
    if (shmid != -1 && shmctl(shmid, IPC_RMID, 0) == -1) {
        fprintf(stderr, "Error: Failed to delete shared memory. %s.\n", strerror(errno));
        retval = EXIT_FAILURE;
    }
    return retval;
}