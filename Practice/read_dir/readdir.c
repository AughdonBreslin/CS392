#include <dirent.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

#define BUFSIZE 128

// LAZY ATTEMPT: Just open current dir.
int main() {
    DIR *dir = opendir(".");
    if (dir == NULL) {
        fprintf(stderr, "Error: Cannot open current directory.\n");
        exit(EXIT_FAILURE);
    }
    char buf[PATH_MAX]; // PATH_MAX includes the \0, so +1 is not required.
    char timebuf[BUFSIZE]; // Print last modified time of the file.
    struct stat sb;
    struct dirent *de;

    while ((de = readdir(dir)) != NULL) {
        char *res = realpath(de->d_name, buf);
        if (res == NULL) {
            fprintf(stderr, "Error: realpath() failed. %s.\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
        if (!stat(buf, &sb)) {
            strftime(timebuf, BUFSIZE, "%b %d %Y %H:%M", localtime(&sb.st_mtime));
            printf("%s [%s]\n", buf, timebuf);
        } else {
            printf("%s\n", buf);
        }
        printf("%s\n", buf);
    }

    closedir(dir);
    exit(EXIT_SUCCESS);
}