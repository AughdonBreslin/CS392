#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

/**
struct stat {
    dev_t     st_dev;        
    ino_t     st_ino;         
    mode_t    st_mode; 
    nlink_t   st_nlink;    
    uid_t     st_uid;      
    gid_t     st_gid;        
    dev_t     st_rdev;        
    off_t     st_size;        
    blksize_t st_blksize;     
    blkcnt_t  st_blocks;      
};
*/

int main(int argc, char *argv[]) {
    struct stat buf;
    char output[128], *ptr;

    for (int i = 1; i < argc; i++) {
        printf("%s: ", argv[i]);
        /* The lstat function is similar to stat, but when the named file is
           a symbolic link, lstat returns information about the symbolic link,
           not the file referenced by the symbolic link. */
        if (lstat(argv[i], &buf) < 0) {
            fprintf(stderr, "Error: Cannot stat '%s'.\n", argv[i]);
            continue;
        }
        // Use macros to see file type
        if (S_ISREG(buf.st_mode)) {
            // Instead of printing to screen or stderror, we can print to string.
            sprintf(output, "Regular, %zu bytes", buf.st_size);
            ptr = output;
        } else if (S_ISDIR(buf.st_mode)) {
            ptr = "directory";
        } else if (S_ISCHR(buf.st_mode)) {
            ptr = "character special";
        } else if (S_ISBLK(buf.st_mode)) {
            ptr = "block special";
        } else if (S_ISFIFO(buf.st_mode)) {
            ptr = "FIFO";
        } else if (S_ISLNK(buf.st_mode)) {
            ptr = "symbolic link";
        } else if (S_ISSOCK(buf.st_mode)) {
            ptr = "socket";
        } else {
            ptr = "** unknown mode **";
        }

        printf("%s\n", ptr);
    }
}