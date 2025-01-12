#include <dirent.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <direcotry>\n", argv[0]);
        return EXIT_FAILURE;
    }

    char path[PATH_MAX];
    if (realpath(argv[1], path) == NULL) {
        fprintf(stderr, "Error: Cannot get full path of file '%s'. %s.\n", 
                argv[1], strerror(errno));
        return EXIT_FAILURE;
    }

    DIR *dir;
    if ((dir = opendir(path)) == NULL) {
        fprintf(stderr, "Error: Cannot open directory. '%s'. %s.\n",
                 path, strerror(errno));
        return EXIT_FAILURE;
    }
    struct dirent *entry;
    struct stat sb;  // stat buff
    char full_filename[PATH_MAX + 2]; // Account for \0
    size_t pathlen = 0;

    // Set the initial character to the NULL byte.
    // If the path is root '/', you can now take the strlen of a properly
    // terminated empty string.
    full_filename[0] = '\0';
    if (strcmp(path, "/")) {
        // If the path is NOT the root - '/', then...

        // If there is no NULL byte among the first n bytes of the path,
        // the full_filename will not be terminated. So, copy up to and including
        // PATH_MAX characters.
        size_t copy_len = strnlen(path, PATH_MAX);
        strncpy(full_filename, path, copy_len);
        full_filename[PATH_MAX] = '\0';
    }
    // Add +1 for the trailing '/' that we are going to add.
    pathlen = strlen(full_filename) + 1;
    full_filename[pathlen - 1] = '/'; // Helpful for deeper directories! Just concat it
    full_filename[pathlen] = '\0';
    // printf("%s\n", full_filename);

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || 
            strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        strncpy(full_filename + pathlen, entry->d_name, PATH_MAX - pathlen);

        // We can check if permissions match here.
        if (lstat(full_filename, &sb) < 0) {
            fprintf(stderr, "Error: Cannot stat file '%s'. %s.\n",
                    full_filename, strerror(errno));
            continue;
        }

        // This code can be used to recurse into directory!
        if (entry->d_type == DT_DIR) {
            printf("%s [directory]\n", full_filename);
        } else {
            printf("%s\n", full_filename);
        }
        printf("%s\n", full_filename);
    }
    closedir(dir);
    return EXIT_SUCCESS;
}