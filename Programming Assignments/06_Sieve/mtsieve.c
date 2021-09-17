/*******************************************************************************
 * Name        : mtsieve.c
 * Author      : Matthew Oyales & Audie Breslin
 * Date        : 1 July 2021
 * Description : Implement the Sieve of Eratosthenes with multithreading.
 * Pledge      : I pledge my honor that I have abided by the Stevens Honor System.
 ******************************************************************************/
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

#include <sys/sysinfo.h>

int total_count = 0;  // Global count of primes.
pthread_mutex_t lock; // Global mutex lock.

typedef struct arg_struct {
    int start;
    int end;
} thread_args;

/**
 * Borrowed from Lab 4 - head.c
 * 
 * Determines whether or not the input string represents a valid integer.
 * A valid integer has an optional minus sign, followed by a series of digits
 * [0-9].
 */
bool is_integer(char *input) {
    int start = 0, len = strlen(input);

    if (len >= 1 && input[0] == '-') {
        if (len < 2) {
            return false;
        }
        start = 1;
    }
    for (int i = start; i < len; i++) {
        if (!isdigit(input[i])) {
            return false;
        }
    }
    return true;
}

/**
 * Borrowed from Lab 4 - head.c
 * 
 * Takes as input a string and an in-out parameter value.
 * If the string can be parsed, the integer value is assigned to the value
 * parameter and true is returned.
 * Otherwise, false is returned and the best attempt to modify the value
 * parameter is made.
 */
bool is_overflow(char *input, int *value) {
    long long long_long_i;
    if (sscanf(input, "%lld", &long_long_i) != 1) {
        return false;
    }
    *value = (int)long_long_i;
    if (long_long_i != (long long)*value) {
        return true;
    }
    return false;
}

/**
 * Count number of digits in a number, to serve as 
 * a helper for the next function, has_threes.
 */
int num_digits(int num) {
	int count = 0;

	while (num != 0) {
		num /= 10;
		count++;
	}

    return count;
}

/**
 * Check that the given prime has at least two digits that are '3'.
 * If the number is two digits or less, false. (because 33 is not prime)
 * Otherwise, check each digit for 3 and ensure there are at least two.
 */
bool has_threes(int p) {
    // We can fail out immediately if number is 2 digits or less.
    if (num_digits(p) < 3) {
        return false;
    }
    int counter = 0; // Store count of 3's found.
    while (p > 0) {
        int curr_digit = p % 10; // Stores the rightmost digit of a number.
        // Is that number 3? If so, add to the count.
        if (curr_digit == 3) {
            counter++;
        }
        // Move onto the next digit.
        p /= 10;
    }
    return (counter >= 2);
}

/**
 * TODO:
 * Application of the sieve algorithm.
 */
void *sieve(void *ptr) {
    thread_args *bob = (thread_args *) ptr; // Cast void to targs pointer.
    int count = 0;                          // Number of satisfactory primes.
    int a = bob->start;                     // Shorthand for start.
    int b = bob->end;                       // Shorthand for end.
    int limit = (int)sqrt(b);               // Limit for low primes.

    /* Standard Sieve & Low Primes */
    bool *low_primes = (bool *)malloc(sizeof(bool *) * limit);
    low_primes[0] = false; // Set first two as false to make things easier.
    low_primes[1] = false;

    // Initialize all elements from 2 up to and including the limit as true.
    for (int i = 2; i <= limit; i++) {
        low_primes[i] = true;
    }

    // Application of standard sieve.
    for (int i = 0; i < sqrt(limit); i++) {
        if (low_primes[i]) {
            for (int j = pow(i,2); j <= limit; j += i) {
                low_primes[j] = false;
            }
        }
    }

    /* High Primes */
    int high_size = b - a + 1;
    bool *high_primes = (bool *)malloc(sizeof(bool *) * high_size);

    // Initialize all to be true.
    for (int i = 0; i <= high_size; i++) {
        high_primes[i] = true;
    }

    // Using 'p' for this loop so we can use 'i' within it.
    // Named to match the names in the pseudocode.
    // Logically, the code follows the algorithm in the document.
    for (int p = 2; p < limit; p++) {
        if (low_primes[p]) {
            int i = ceil((double)a/p) * p - a;
            if (a <= p) {
                i += p;
            }
            while (i < high_size) {
                high_primes[i] = false;
                i += p;
            }
        }
    }

    /* Check for primes that have at least two '3' digits in them. */
    for (int i = 2; i < high_size; i++) {
        if (high_primes[i]) {
            if (has_threes(i + a)) {
                count++;
            }
        }
    }

    /* Increment the global variable. */
    int retval;
    if ((retval = pthread_mutex_lock(&lock)) != 0) {
        fprintf(stderr, "Warning: Cannot lock mutex. %s.\n",
                strerror(retval));
    }

    total_count += count;

    if ((retval = pthread_mutex_unlock(&lock)) != 0) {
        fprintf(stderr, "Warning: Cannot unlock mutex. %s.\n",
                strerror(retval));
    }

    free(low_primes);
    free(high_primes);
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc == 1) {
        fprintf(stderr, "Usage: ./mtsieve -s <starting value> -e <ending value> -t <num threads>\n");
        return EXIT_FAILURE;
    }

    // Variables to hold string input data.
    char *start_val;
    char *end_val;
    char *num_threads;

    // Variables to store numerical data from input.
    int start = 0;
    int end = 0;
    int thread_count = 0;

    // Flags to track options have been given.
    int s = 0;
    int e = 0;
    int t = 0;

    // Option variables
    int c;
    opterr = 0;

    /**
     * Error checking is going to be kind of annoying.
     * Within the getopt loop, we need to check if any argument is not an integer or 
     * if the number is too large (integer overflow)
     * 
     * Not an integer  : "Error: Invalid input '%s' received for parameter '-%c'.\n"
     * Integer overflow: "Error: Integer overflow for parameter '-%c'.\n"
     */
    while ((c = getopt(argc, argv, "s:e:t:")) != -1) {
        switch (c) {
            case 's':
                s = 1;
                start_val = optarg;
                if (!is_integer(start_val)) {
                    fprintf(stderr, "Error: Invalid input '%s' received for parameter '-%c'.\n",
                            start_val, c);
                    return EXIT_FAILURE;
                } else if (is_overflow(start_val, &start)) {
                    fprintf(stderr, "Error: Integer overflow for parameter '-%c'.\n", c);
                    return EXIT_FAILURE;
                }
                break;
            case 'e':
                e = 1;
                end_val = optarg;
                if (!is_integer(end_val)) {
                    fprintf(stderr, "Error: Invalid input '%s' received for parameter '-%c'.\n",
                            end_val, c);
                    return EXIT_FAILURE;
                } else if (is_overflow(end_val, &end)) {
                    fprintf(stderr, "Error: Integer overflow for parameter '-%c'.\n", c);
                    return EXIT_FAILURE;
                }
                break;
            case 't':
                t = 1;
                num_threads = optarg;
                if (!is_integer(num_threads)) {
                    fprintf(stderr, "Error: Invalid input '%s' received for parameter '-%c'.\n",
                            num_threads, c);
                    return EXIT_FAILURE;
                } else if (is_overflow(num_threads, &thread_count)) {
                    fprintf(stderr, "Error: Integer overflow for parameter '-%c'.\n", c);
                    return EXIT_FAILURE;
                }
                break;
            case '?':
                if (optopt == 'e' || optopt == 's' || optopt == 't') {
                    fprintf(stderr, "Error: Option -%c requires an argument.\n", optopt);
                } else if (isprint(optopt)) {
                    fprintf(stderr, "Error: Unknown option '-%c'.\n", optopt);
                } 
                else {
                    fprintf(stderr, "Error: Unknown optiofn character '\\x%x'.\n", optopt);
                }
                return EXIT_FAILURE;
            default:
                break;
        }
    }

    /**
     * After processing all arguments in the getopt loop, check conditions:
     * 
     * 1. "Error: Non-option argument '%s' supplied.\n"
     * 2. "Error: Required argument <starting value> is missing.\n"
     * 3. "Error: Starting value must be >= 2.\n"
     * 4. "Error: Required argument <ending value> is missing.\n"
     * 5. "Error: Ending value must be >= 2.\n"
     * 6. "Error: Ending value must be >= starting value.\n"
     * 7. "Error: Required argument <num threads> is missing.\n"
     * 8. "Error: Number of threads cannot be less than 1.\n"
     * 9. "Error: Number of threads cannot exceed twice the number of processors(%d).\n" (man 3 get_nprocs)
     */

    // Case 1
    // WTF IT WAS THIS SIMPLE??? optind man...
    if (optind < argc) {
        fprintf(stderr, "Error: Non-option argument '%s' supplied.\n", argv[optind]);
        return EXIT_FAILURE;
    }

    // Case 2 & 3.
    if (!s) {
        fprintf(stderr, "Error: Required argument <starting value> is missing.\n");
        return EXIT_FAILURE;
    } else if (start < 2) {
        fprintf(stderr, "Error: Starting value must be >= 2.\n");
        return EXIT_FAILURE;
    }
    
    // Case 4, 5, 6
    if (!e) {
        fprintf(stderr, "Error: Required argument <ending value> is missing.\n");
        return EXIT_FAILURE;
    } else if (end < 2) {
        fprintf(stderr, "Error: Ending value must be >= 2.\n");
        return EXIT_FAILURE;
    } else if (end < start) {
        fprintf(stderr, "Error: Ending value must be >= starting value.\n");
        return EXIT_FAILURE;
    }
    
    // Case 7, 8, 9
    int processors = get_nprocs();
    if (!t) {
        fprintf(stderr, "Error: Required argument <num threads> is missing.\n");
        return EXIT_FAILURE;
    } else if (thread_count < 1) {
        fprintf(stderr, "Error: Number of threads cannot be less than 1.\n");
        return EXIT_FAILURE;
    } else if (thread_count > processors * 2) {
        fprintf(stderr, "Error: Number of threads cannot exceed twice the number of processors(%d).\n", processors);
        return EXIT_FAILURE;
    }

    /**
     *  Segment the range of values.
     * 
     *  First, compute how many numbers are being tested for primality. If the 
     *  number of threads exceeds the count, reduce the number of threads to 
     *  match the count.
     * 
     *  Otherwise, take the count and divide it by the number of threads the user requested to create. 
     *  Each thread will process at least that many numbers. Take the remainder and distribute it among
     *  all the threads. Unless the thread number divides the count evenly, the later segments will
     *  contain less values.
     */
    int num_tested = end - start + 1;     // How many numbers being tested for primality?             

    if (thread_count > num_tested) {
        thread_count = num_tested;        // Reduce no. of threads to match count.
    }

    int npt = num_tested / thread_count;  // Numbers per thread.
    int remainder = num_tested % thread_count;

    /**
     * TODO: Start the appropriate number of threads and have each run algorithm over
     *       the desired range.
     * 
     * As each thread finds prime numbers meeting the condition, it will increment the 
     * global variable total_count. Of course, to guarantee that no race conditions occur, 
     * proper use of the mutex must be ensured.
     */
    printf("Finding all prime numbers between %d and %d.\n", start, end);

    int retval;
    if ((retval = pthread_mutex_init(&lock, NULL)) != 0) {
        fprintf(stderr, "Error: Cannot create mutex. %s.\n", strerror(retval));
        return EXIT_FAILURE;
    }

    pthread_t threads[thread_count];  // Array of threads
    thread_args targs[thread_count];  // Array of targs

    // Create the threads
    int st = start;
    for (int i = 0; i < thread_count; i++) {
        if(remainder == 0) {
            npt--;
        }
        remainder--;
        targs[i].start = st;
        targs[i].end = st + npt;
        st += npt + 1;

        int retval;
        if ((retval = pthread_create(&threads[i], NULL, sieve, &targs[i])) != 0) {
            fprintf(stderr, "Error: Cannot create thread %d. %s.\n",
                    i+1, strerror(retval));
            return EXIT_FAILURE;
        }
    }

    for (int i = 0; i < thread_count; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            fprintf(stderr, "Warning: Thread %d did not join properly.\n", i+1);
        }
    }

    if ((retval = pthread_mutex_destroy(&lock)) != 0) {
        fprintf(stderr, "Warning: Cannot destroy mutex. %s.\n", strerror(retval));
    }

    /**
     * Output printing section
     * 
     * "Total primes between %d and %d with two or more '3' digits: %d\n"
     * 
     * where the first %d is the starting value, 
     * the second %d is the ending value, 
     * and the third %d is the count.
     */

    // Grammar check!
    (thread_count == 1) ? 
        printf("%d segment:\n", thread_count) : printf("%d segments:\n", thread_count);

    // Display all segments.
    for (int i = 0; i < thread_count; i++) {
        printf("   [%d, %d]\n", targs[i].start, targs[i].end);
    }

    // Output result, as specified in document.
    printf("Total primes between %d and %d with two or more '3' digits: %d\n",
            start, end, total_count);

    return EXIT_SUCCESS;
}