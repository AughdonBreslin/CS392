#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define SCALE_FACTOR 1000000

int num_running = 0;
pthread_mutex_t lock;

int random_int_in_range(int low, int high) {
    return low + rand() % (high - low + 1);
}

// CRITICAL SECTION
void *sleep_print(void *ptr) {
    int thread_num = *(int *)ptr;
    long tid = (long)pthread_self();
    srand((time(NULL) & 0xFFFF) | (tid << 16));


    useconds_t sleep_time = (useconds_t)random_int_in_range(1 * SCALE_FACTOR, 2 * SCALE_FACTOR);
    printf("I, thread %d, am going to sleep for %.2f seconds.\n", thread_num, (double)sleep_time / SCALE_FACTOR);
    usleep(sleep_time);

    printf("I, thread %d is have finished.\n", *(int *)ptr);

    int retval;
    if ((retval = pthread_mutex_lock(&lock)) != 0) {
        fprintf(stderr, "Warning: Cannot lock mutex. %s.\n", strerror(retval));
    }
    num_running--;
    if ((retval = pthread_mutex_unlock(&lock)) != 0) {
        fprintf(stderr, "Warning: Cannot unlock mutex. %s.\n", strerror(retval));
    }
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <num_threads>\n", argv[0]);
        return EXIT_FAILURE;
    }
    int num_threads = atoi(argv[1]);
    if (num_threads <= 0) {
        fprintf(stderr, "Error: Invalid number of threads '%s'.\n", argv[1]);
        return EXIT_FAILURE;
    }

    pthread_t *threads;
    if ((threads = (pthread_t *)malloc(num_threads * sizeof(pthread_t))) == NULL) {
        fprintf(stderr, "Error: Cannot allocate memory for threads.\n");
        return EXIT_FAILURE;
    }

    int *thread_nums;
    if ((thread_nums = (int *)malloc(num_threads * sizeof(int))) == NULL) {
        free(threads);
        fprintf(stderr, "Error: Cannot allocate memory for thread args.\n");
        return EXIT_FAILURE;
    }

    // Place the mutex here.
    int retval;
    if ((retval = pthread_mutex_init(&lock, NULL)) != 0) {
        fprintf(stderr, "Error: Cannot create mutex. %s.\n", strerror(retval));
        free(threads);
        free(thread_nums);
        return EXIT_FAILURE;
    }

    // Create independent threads.
    int num_started = 0;
    for (int i =0; i < num_threads; i++) {
        thread_nums[i] = i+1;
        int retval;
        if ((retval = pthread_create(&threads[i], NULL, sleep_print,
                                     (void *)(&thread_nums[i]))) != 0) {
            fprintf(stderr, "Error: Cannot create threads %d. "
                            "No more threads will be created. %s.\n",
                            i + 1, strerror(retval));
            break;
        }
        num_started++;

        if ((retval = pthread_mutex_lock(&lock)) != 0) {
            fprintf(stderr, "Warning: Cannot lock mutex. %s.\n", strerror(retval));
        }
        // Need to surround with the mutex
        num_running++;
        if ((retval = pthread_mutex_unlock(&lock)) != 0) {
            fprintf(stderr, "Warning: Cannot unlock mutex. %s.\n", strerror(retval));
        }
    }
    
    // We did not join the threads in this example, so you might not get to
    // see all threads having executed.
    for (int i = 0; i < num_started; i++) {
        int retval;
        if ((retval = pthread_join(threads[i], NULL)) != 0) {
            fprintf(stderr, "Warning: cannot join threads %d.\n", i+1);
            break;
        }
    }
    printf("Num threads started: %d\nThreads still running: %d\n", num_started, num_running);

    free(threads);
    free(thread_nums);

    if ((retval = pthread_mutex_destroy(&lock)) != 0) {
        fprintf(stderr, "Warning: Cannot destroy mutex. %s.\n", strerror(retval));
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}