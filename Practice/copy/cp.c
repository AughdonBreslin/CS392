#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFSIZE 128

void display_usage(char *progname) {
    printf("Usage: %s <source> <destination>\n", progname);
}

/**
 * This program trivially copies a file from one location to another.
 * Uses stat to determine if the source and destination files are the same, and
 * errors out in that case to prevent erasing the contents of the file.
 */
int main(int argc, char *argv[]) {
    if (argc != 3) {
        display_usage(argv[0]);
        return EXIT_FAILURE;
    }

    /* Use stat to get information about the files. */
    struct stat src_sb;
    if (stat(argv[1], &src_sb) < 0) {
        fprintf(stderr, "Error: Cannot stat source file '%s'. %s.\n",
                argv[1], strerror(errno));
        return EXIT_FAILURE;
    }
    struct stat dst_sb;
    int retval = stat(argv[2], &dst_sb);
    if (retval == 0 && src_sb.st_ino == dst_sb.st_ino) {
        fprintf(stderr, "Error: Source and destination files are the same.\n");
        return EXIT_FAILURE;
    }

    /* Open the source file for reading. */
    int src_fd;
    if ((src_fd = open(argv[1], O_RDONLY)) == -1) {
        fprintf(stderr, "Error: Cannot open source file '%s'. %s.\n",
                argv[1], strerror(errno));
        return EXIT_FAILURE;
    }

    /* Open the destination file for writing. */
    int dst_fd;
    if ((dst_fd =
          open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, src_sb.st_mode)) == -1) {
        fprintf(stderr, "Error: Cannot open source file '%s'. %s.\n",
                argv[2], strerror(errno));
        return EXIT_FAILURE;
    }

    /* Allocate a buffer on the heap. */
    char *buf;
    if ((buf = (char *)malloc(sizeof(char) * BUFSIZE)) == NULL) {
        fprintf(stderr, "Error: malloc failed.\n");
        close(src_fd);
        close(dst_fd);
        return EXIT_FAILURE;
    }

    /* Read from the source file into the buffer, and write from the buffer into
       the destination file. */
    int bytes_read;
    while ((bytes_read = read(src_fd, buf, BUFSIZE)) > 0) {
        if (write(dst_fd, buf, bytes_read) != bytes_read) {
            fprintf(stderr, "Error: Failed to write to file '%s'. %s.\n",
                    argv[2], strerror(errno));
            free(buf);
            close(src_fd);
            close(dst_fd);
            return EXIT_FAILURE;
        }
    }

    /* Make sure read did not fail. */
    if (bytes_read == -1) {
        fprintf(stderr, "Error: Failed to read from file '%s'. %s.\n",
                argv[1], strerror(errno));
    }

    /* Clean up. */
    free(buf);
    close(src_fd);
    close(dst_fd);

    return EXIT_SUCCESS;
}
