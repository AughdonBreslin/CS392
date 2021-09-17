#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int makearg(char string[], char ***args) {
    char *bufferNext;                                      // pointer to the lines after the space
    int static count = 0;                                  // static count (recurssion)
     
    bufferNext = strchr(string, ' ');                      // find a space in string
 
 
    if (bufferNext != NULL) {    
         
        *bufferNext++ = '\0';                               // replace the space with a nul char
        args = malloc(sizeof(char)*strlen(string));        // allocate enough space to store the string
        *args = string;                                    // store the string    
         
        if (count == 0)    
            printf("You called the command %s\n", *args);  // tell user the command called
        else
            printf("Argument %d: %s\n", count, *args);     // tell user the arguments
     
        count++;                                           // increment the counter
 
 
        makearg(bufferNext, args++);                       // more spaces possible, continue 
    } else { 
        args = malloc(sizeof(char)*strlen(string));        // allocate for the last argument 
        *args = string;                                    // store the last argument
        printf("Argument %d: %s\n", count, *args);         // print the last argument
    }
    args = args - count;                                   // reset the args pointer to the beggining
    return count;
}

int main(int argc, char **argv) {
    int argcount;
    char **args, str[] = "ls -l file";
    argcount = makearg(str, &args);
    printf("There were a total of %d arguments.\n", argcount);
     
    int i = 0;
    for (i = 0; i < argcount; i++) {
            printf("%s\n", *args[i]);
    }
     
    return 0;
}