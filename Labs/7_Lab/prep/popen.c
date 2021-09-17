#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main() {
    FILE *pipe;
    FILE *f;
    int exit_status;
    unsigned long bytes_read;
    char buf[128];

    // A very simple way to let the code decide how to open and manage processes.
    if ((pipe = popen("wc -w", "w")) == NULL) {
        fprintf(stderr, "Error: failed to open pipe. %s.\n", strerror(errno));
        return EXIT_FAILURE;
    }

    // Open up "foo.txt" for reading.
    if ((f = fopen("foo.txt", "r")) == NULL) {
        fprintf(stderr, "Error: failed to open file. %s.\n", strerror(errno));
        return EXIT_FAILURE;
    }

    // Read in sizes of 127 characters from the file into buf,
    // then write the number of bytes read into the pipe.
    while ((bytes_read = fread(buf, sizeof(char), 127, f)) > 0) {
        if (bytes_read != fwrite(buf, sizeof(char), bytes_read, pipe)) {
            fprintf(stderr, "Error: failed to write to pipe.\n");
        }
    }

    // When fread returns 0, it means there was an error or reached end of file.
    // This can be checked with ferror or feof.
    // One will return true while the other returns false depending on the situation that occurred.
    if (ferror(f)) {
        fprintf(stderr, "Error: failed to properly read from file.\n");
    }

    // We can't forget to close the file or the pipe.
    // Close the file.
    if (fclose(f) < 0) {
        fprintf(stderr, "Error: failed to close file. %s.\n", strerror(errno));
        return EXIT_FAILURE;
    }

    // Close the pipe.
    if ((exit_status = pclose(pipe)) < 0) {
        fprintf(stderr, "Error: failed to close pipe. %s.\n", strerror(errno));
        return EXIT_FAILURE;
    } else {
        return exit_status;
    }
    // When the pipe is closed, the child process receives EOF through standard in, and terminates.
    // pclose returns the return status of the child process.
}