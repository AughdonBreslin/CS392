#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

bool starts_with(const char *str, const char *prefix) {
    /* TODO:
       Return true if the string starts with prefix, false otherwise.
       Note that prefix might be longer than the string itself.
     */
    if(strlen(prefix) > strlen(str)) {
      fprintf(stderr, "Error: Prefix '%s' is longer than string '%s'. %s.\n",
          str,prefix,strerror(errno));
    }
    /* "AT THE VERY WORST" THANKS DR B >:(*/
    for(int i = 0; i < strlen(prefix); i++) {
      if(prefix[i] != str[i]) {
        return false;
      }
    }
    return true;

}

int main() {
    /* TODO:
       Open "cat /proc/cpuinfo" for reading, redirecting errors to /dev/null.
       If it fails, print the string "Error: popen() failed. %s.\n", where
       %s is strerror(errno) and return EXIT_FAILURE.
     */
    FILE *file;
    if ((file = popen("cat /proc/cpuinfo 2>/dev/null","r")) == NULL) {
        fprintf(stderr, "Error: popen() failed. %s.\n", strerror(errno));
        return EXIT_FAILURE;
    }


    /* TODO:
       Allocate an array of 256 characters on the stack.
       Use fgets to read line by line.
       If the line begins with "model name", print everything that comes after
       ": ".
       For example, with the line:
       model name      : AMD Ryzen 9 3900X 12-Core Processor
       print
       AMD Ryzen 9 3900X 12-Core Processor
       including the new line character.
       After you've printed it once, break the loop.
     */
    char buf[256];
    char* line;
    while ((line = fgets(buf, 256, file)) != NULL) {
      if(starts_with(line,"model name")){
        char* start = strstr(line, ": ");
        printf("%s", start+2);
        break;
      }
    } 


    /* TODO:
       Close the file descriptor and check the status.
       If closing the descriptor fails, print the string
       "Error: pclose() failed. %s.\n", where %s is strerror(errno) and return
       EXIT_FAILURE.
     */
    if (ferror(file)) {
      fprintf(stderr, "Error: failed to properly read from file.\n");
    }
    int status = 0;
    if ((status = pclose(file)) < 0) {
      fprintf(stderr, "Error: pclose() failed. %s.\n", strerror(errno));
      return EXIT_FAILURE;
    }
    return !(WIFEXITED(status) && WEXITSTATUS(status) == EXIT_SUCCESS);
}
