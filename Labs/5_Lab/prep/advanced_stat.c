#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

int main() {
    int my_fd, bytes_written;
    char *initial_contents = "this is the contents of myfile\n";

    if ((my_fd = open("myfile.txt", O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR)) < 0) {
        fprintf(stderr, "Failed to create file: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }

    bytes_written = write(my_fd, initial_contents, strlen(initial_contents));
    close(my_fd);
    
    if (bytes_written < 0) {
        fprintf(stderr, "Failed to write to file: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }
    // remaining code snippets will be put in here
    struct stat statbuf;

    // We had to reference statbuf to fill it.
    // We want to get the address of statbuf?
    if (stat("myfile.txt", &statbuf) < 0) {
        fprintf(stderr, "Failed to get information on the file: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }
    // After this, statbuf has info about the file.
    // Refer to "man 2 stat" to see what can be obtained:
    /*
       dev_t     st_dev;         ID of device containing file
       ino_t     st_ino;         Inode number
       mode_t    st_mode;        File type and mode
       nlink_t   st_nlink;       Number of hard links
       uid_t     st_uid;         User ID of owner
       gid_t     st_gid;         Group ID of owner
       dev_t     st_rdev;        Device ID (if special file)
       off_t     st_size;        Total size, in bytes
       blksize_t st_blksize;     Block size for filesystem I/O
       blkcnt_t  st_blocks;      Number of 512B blocks allocated
     */

    // What is the size of the file?
    printf("Size of file: %lu characters\n", statbuf.st_size);

    // What is the user ID of the owner of the file?
    printf("File owner's UID: %u\n", statbuf.st_uid);

    // When was the file last accessed/modified?
    // Refer to "man 3 ctime" and be sure to #include <time.h>
    printf("Last access time: %s", ctime(&statbuf.st_atime));
    printf("Last modification time: %s", ctime(&statbuf.st_mtime));

    // How to find the permissions of the file?
    printf("Permissions of file (not human-readable!): %d\n", statbuf.st_mode);
    // Permissions of file are just numbers! How to properly read into this?
    // For myfile.txt, the numbers seem to be "33272" (could be diff. elsewhere idk)

    /** 
       The breakdown of file permissions:
       | S_IRUSR    | S_IWUSR     | S_IXUSR       |
       | Read owner | Write owner | Execute owner |
       |            |             |               |
       | S_IRGRP    | S_IWGRP     | S_IXGRP       |
       | Read group | Write group | Execute group |
       |            |             |               |
       | S_IROTH    | S_IWOTH     | S_IXOTH       |
       | Read other | Write other | Execute other |

       They are declarations w/ values some no. of bits to the left
       We can use them to figure out the values of different permission bits in the statbuf’s
       mode field, with a bitwise AND.
    */

    // Greater than 0 if owner can read from file, and zero if not.
    // int user_read_permissions = statbuf.st_mode & S_IRUSR;

    int perms[] = {S_IRUSR, S_IWUSR, S_IXUSR, S_IRGRP, S_IWGRP, S_IXGRP, S_IROTH, S_IWOTH, S_IXOTH};
    int permission_valid; // Or a boolean if you want to do that.
    printf("Permissions as they appear in `ls -l`: ");
    printf("-");; // print file type bit, as it appears in `$ ls - l`. Obviously different for dirs or blocks, etc.
    for (int i = 0; i < 9; i += 3) {
        permission_valid = statbuf.st_mode & perms[i];
        if (permission_valid) {
            printf("r");
        } else {
            printf("-");
        }
        permission_valid = statbuf.st_mode & perms[i+1];
        if (permission_valid) {
            printf("w");
        } else {
            printf("-");
        }
        permission_valid = statbuf.st_mode & perms[i+2];
        if (permission_valid) {
            printf("x");
        } else {
            printf("-");
        }
    }
    printf("\n");

    return EXIT_SUCCESS;
}
