/*******************************************************************************
 * Name        : pfind.c
 * Author      : Matthew Oyales & Audie Breslin
 * Date        : 12 June 2021
 * Description : Permission Find
 * Pledge      : I pledge my honor that I have abided by the Stevens Honor System.
 ******************************************************************************/
#include <sys/stat.h>
#include <sys/types.h>

#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void print_usage(char *progname) {
    printf("Usage: %s -d <directory> -p <permissions string> [-h]\n", progname);
}

/** A quick print function to shorten the error writing in the main(). */
void bad_str(char *str_name) {
    fprintf(stderr, "Error: Permissions string '%s' is invalid.\n", str_name);
}

/**
 * THIS IS SO CLUTCH
 * 
 * The below function takes in the current st_mode of the sb, and bitwise &s against
 * the permission macros. 
 * - We loop over the entire permission string.
 * - If (mode & perms[i]) == 0, permission is not valid. But if ps_arr[i] is 1, this is a mismatch.
 * - If (mode & perms[i]) > 0, permission is valid. But if ps_arr[i] is 0, this too is a mismatch.
 * - If we manage to finish the loop, then we know the file permissions match and we print!
 */
bool perms_match(mode_t mode, int ps_arr[]) {
    int perms[] = {S_IRUSR, S_IWUSR, S_IXUSR,
                   S_IRGRP, S_IWGRP, S_IXGRP,
                   S_IROTH, S_IWOTH, S_IXOTH};

    for (int i = 0; i < 9; i++) {
        if ((mode & perms[i]) == 0 && ps_arr[i] == 1) {
            return false;
        }
        if ((mode & perms[i]) > 0 && ps_arr[i] == 0) {
            return false;
        }
    }

    return true;
}

int search_files(char *dir, int ps_resolved[]) {
    DIR *dp;

    // Attempt to open the current directory.
    if ((dp = opendir(dir)) == NULL) {
        fprintf(stderr, "Error: Cannot open directory '%s'. %s.\n", dir, strerror(errno));
        closedir(dp);
        return EXIT_FAILURE;
    }

    struct dirent *dent;
    struct stat sb;
    char full_filename[PATH_MAX];
    size_t pathlen = 0;

    full_filename[0] = '\0';
    if (strcmp(dir, "/")) {
        strncpy(full_filename, dir, PATH_MAX);
    }

    pathlen = strlen(full_filename) + 1;
    full_filename[pathlen - 1] = '/';
    full_filename[pathlen] = '\0';

    while ((dent = readdir(dp)) != NULL) {
        if (strcmp(dent->d_name, ".") == 0 ||
            strcmp(dent->d_name, "..") == 0) {
            continue;
        }

        strncpy(full_filename + pathlen, dent->d_name, PATH_MAX - pathlen);

        if (lstat(full_filename, &sb) < 0) {
            fprintf(stderr, "Error: Cannot stat file '%s'. %s. \n",
                    full_filename, strerror(errno));
            continue;
        }

        if(dent->d_type == DT_DIR) {
            if (perms_match(sb.st_mode, ps_resolved)) {
                printf("%s\n", full_filename);
            }
            search_files(full_filename, ps_resolved);
        } else {
            if (perms_match(sb.st_mode, ps_resolved)) {
                printf("%s\n", full_filename);
            }    
        }
    }
    closedir(dp);

    return EXIT_SUCCESS;
}

int main(int argc, char *argv[]) {

    // No input arguments (confirmed this case against the provided binary.)
    if (argc == 1) {
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    char *dir_name;     // Save whatever comes after -d
    char *perm_string;  // Save whatever comes after -p

    int dflag = 0;
    int pflag = 0;
    int c;

    opterr = 0;         // Suppress regular optarg error message

    while ((c = getopt(argc, argv, ":d:p:h")) != -1) {
        switch (c) {
        case 'd':
            if (!optarg) {
                fprintf(stderr, "Error: Required argument -d <directory> not found.\n");
                return EXIT_FAILURE;
            }
            dflag = 1;
            dir_name = optarg;
            break;
        case 'p':
            pflag = 1;
            perm_string = optarg;
            break;
        case 'h':
            print_usage(argv[0]);;
            return EXIT_SUCCESS;
        case '?':
            //NOTE: Not sure what this if and the else are used for
            if (optopt == 'c')
                fprintf(stderr, "Option -%c requires an argument.\n", optopt);
            else if (isprint(optopt)) {
                fprintf(stderr, "Error: Unknown option '-%c' received.\n", optopt);
                return EXIT_FAILURE;
            }
            else
                fprintf(stderr,
                        "Unknown option character `\\x%x'.\n",
                        optopt);
            return EXIT_FAILURE;
        default:
            break;
        }
    }

    /**
     * CHECK FOR COMMAND-LINE ERRORS
     * 
     * Should check for:
     * - If both flags are not activated.
     * - If neither flag has an argument after it
     * - If the directory specifed does not exist
     * 
     * Validating the permission string comes after this block.
     */
    if (!dflag) {
        fprintf(stderr, "Error: Required argument -d <directory> not found.\n");
        return EXIT_FAILURE;
    } else if (!pflag) {
        fprintf(stderr, "Error: Required argument -p <permissions string> not found.\n");
        return EXIT_FAILURE;
    }

    // Really cheesy solution to check if the input directory is unable to be accessed.
    DIR *dp = opendir(dir_name);

    char path[PATH_MAX];
    if (realpath(dir_name, path) == NULL) {
        fprintf(stderr, "Error: Cannot stat '%s'. %s.\n", dir_name, strerror(errno));
        free(dp);
        return EXIT_FAILURE;
    }

    if (dp == NULL && errno == EACCES) {
        fprintf(stderr, "Error: Cannot open directory '%s'. %s.\n", path, strerror(errno));
        closedir(dp);
        return EXIT_FAILURE;
    } else if (dp == NULL) {
        fprintf(stderr, "Error: Cannot stat '%s'. %s.\n", path, strerror(errno));
        closedir(dp);
        return EXIT_FAILURE;
    }

    size_t str_size = strlen(perm_string);

    /**
     * VALIDATING THE PERMISSION STRING
     * 
     * This fuckin bodge of a section of code validates perm_string and its contents,
     * verifying that it is both the right length and that the right character are in
     * the right place. You may notice the logic is quite similar to permstat.c, which
     * was intended because it seemed like the best way to check every part of the string
     * in a much more concise manner.
     */
    if (str_size != 9) {
        // Length requirement not satisfied.
        free(dp);
        bad_str(perm_string);
        return EXIT_FAILURE;
    } else {
        // Check that characters are in correct position.
        for (int i = 0; i < str_size; i += 3) {
            if (perm_string[i] != 'r' && perm_string[i] != '-') {
                bad_str(perm_string);
                free(dp);
                return EXIT_FAILURE;
            } 
            if (perm_string[i+1] != 'w' && perm_string[i+1] != '-') {
                bad_str(perm_string);
                free(dp);
                return EXIT_FAILURE;
            }
            if (perm_string[i+2] != 'x' && perm_string[i+2] != '-') {
                bad_str(perm_string);
                free(dp);
                return EXIT_FAILURE;
            }
        }
    }

    /**
     * RESOLVING THE PERMISSION STRING
     * 
     * After we have verified that the string is good in terms of its actual letters,
     * we have to turn it into an integer such that the binary corresponds to the
     * active permissions.
     * 
     * Example:
     * rw-r--r--
     * 110100100 (fun trivia: the whole thing is 420 in decimal)
     * Yes, I know it's technically supposed to be 644, but look at the assignment doc.
     */
    int ps_resolved[9];                // Make an array of binary bits
    for (int i = 0; i < 9; i++) {
        if (perm_string[i] == '-') {
            ps_resolved[i] = 0;
        } else {
            ps_resolved[i] = 1;
        }
    }

    /**
     * RECURSIVELY NAVIGATE DIRECTORY TREE
     * 
     * Now that the input has been checked and the permission string resolved, it's
     * time to actually check the directory and matching permissions against the target.
     */
    
    // NO MEMORY DIARRHEA - recall that dp ends up on the heap!
    free(dp);
    return search_files(path, ps_resolved);
}