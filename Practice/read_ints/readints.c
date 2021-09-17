#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFSIZE 128

/**
 * Determines whether or not the input string represents a valid integer.
 * A valid integer has an optional minus sign, followed by a series of digits.
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
 * Takes as input a string and an in-put parameter value.
 * If the string can be parsed, the integer value is assigned to the value parameter
 * and true is returned.
 * Otherwise, false is returned and the best attempt to modify the value parameter is made.
 */
bool get_integer(char *input, int *value) {
    long long long_long_i;
    if (sscanf(input, "%lld", &long_long_i) != 1) {
        return false;
    }
    *value = (int) long_long_i; // Hacky way to check if overflow
    // Casting down and casting back up will check if we have overflow!
    if (long_long_i == (long long)*value) {
        fprintf(stderr, "Warning: Integer overflow with '%s'.\n", input);
        return false;
    }
    return true;
} 

int main(int argc, char *argv[]) {
    if (argc != 2) {
        // fprintf will print to standard error.
        // Specify in the first argument that we want to print to stderror.
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return EXIT_FAILURE;
    }
    char buf[BUFSIZE + 2];
    /** 
     * fopen returns a pointer to a FILE (all caps required)
     * Recall that argv[1] is just a string (or char*), but what is returned is 
     * a pointer to the actual file and its contents
     */
    FILE *fp = fopen(argv[1], "r");
    if (fp == NULL) {
        // Tell you specifically what went wrong when trying to open the file.
        fprintf(stderr, "Error: Cannot open file %s. %s.\n", argv[1],
                 strerror(errno));
        return EXIT_FAILURE;
    }

    /** 
     * Reads at most n-1 characters from fp until a newline is found. The
     * characters up to and including the newline are stored in buf.
     * The buffer is terminated with a '\0'.
     * 
     * The buffer only reads one line at a time.
     */
    while (fgets(buf, BUFSIZE + 2, fp)) {
        // Replace the '\n' with '\0'
        // We would like just the data stored to avoid parsing extra characters.
        // Even if there already is a \0 after, string functions terminate at first \0
        char *eoln = strchr(buf, '\n');
        if (eoln == NULL) {
            // This should not happen.
            // If we don't know, we will just null terminate at the last spot.
            buf[BUFSIZE] = '\0';
        } else {
            // Put a \0 at the end of the array of characters.
            *eoln = '\0';
        }
        if (is_integer(buf)) {
            int val;
            if (get_integer(buf, &val)) {
                printf("%d\n", val);
            }
        }
    }
    fclose(fp);

    return EXIT_SUCCESS;
}