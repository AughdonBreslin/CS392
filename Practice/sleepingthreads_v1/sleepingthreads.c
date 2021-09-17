#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void *sleep_print(void *ptr) {
    printf("Thread %d is running.\n", *(int *)ptr);
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
    }
    printf("Num threads started: %d\n", num_started);

    // We did not join the threads in this example, so you might not get to
    // see all threads having executed.
    for (int i = 0; i < num_started; i++) {
        int retval;
        if ((retval = pthread_join(threads[i], NULL)) != 0) {
            fprintf(stderr, "Warning: cannot join threads %d.\n", i+1);
            break;
        }
    }

    free(threads);
    free(thread_nums);

    return EXIT_SUCCESS;
}