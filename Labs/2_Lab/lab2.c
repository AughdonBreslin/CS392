/*******************************************************************************
 * Name        : lab2.c
 * Author      : Matthew Oyales & Audie Breslin
 * Date        : 2 June 2021
 * Description : Practice with pointers and strings.
 * Pledge      : I pledge my honor that I have abided by the Stevens Honor System.
 ******************************************************************************/
#include <stdio.h>
#include <stdlib.h>

#define BUFLEN 128

/* Function prototypes */
size_t my_strlen(char *src);
char *my_strcpy(char *dst, char *src);

/**
 * Hi Audie, welcome back from Florida!
 * Attempted some stuff in the main function and the implementation outlines for the two functions
 * below. I do not know if this is correct, but you can be my auditor for this,
 * We can look into it further when we talk!
 * 
 * It is very close to strings.c, but there seem to be some differences we should look out for.
 * At the moment, a lot of code is similar to that file, so we can look back at that to check.
 */

/**
 * Accepts user input from standard in, copies it to a new string, and prints
 * out the copied string and string length. You are NOT ALLOWED to use any 
 * functions from <string.h>.
 */
int main() {
    char src[BUFLEN]; // String with at most 128 characters.
    src[0] = '\0';    // First character in the string is the null terminating(?)
    char *copy;       // Declare a pointer to the start of the copy string array.

    printf("Enter a string: ");

    if (scanf("%[^\n]", src) < 0) {
        fprintf(stderr, "Error: Failed to get a string from standard in.\n");
        return EXIT_FAILURE;
    }

    /* TODO: Implement the following features before the return. */

    /* 1. Using malloc, allocate enough space in 'copy' to fit 'src'.
          (man 3 malloc) */
    size_t len = my_strlen(src) + 1;  // Get the length of the string, so we know how much to allocate for 'src'.
                                    //+1 to incorporate the \0 character
    copy = (char *)malloc(len * sizeof(char));
    //printf("size of copy: %ld \n",sizeof(copy));

    /* TODO: These function calls should not crash your program if you handled
             NULLs correctly. Do not delete them! */
    my_strlen(NULL);
    if (my_strcpy(copy, NULL)) {
        fprintf(stderr, "Error: This line should not have executed!\n");
    }

    /* 2. Call my_strcpy to make a copy of src, stored in copy. */
    // I think this is correct? But if this is wrong, then everything else below will not work.
    copy = my_strcpy(copy, src);

    /*
     * 3. Print out the following, matching the format verbatim:
     * Duplicated string: some_string (the copy string)
     * Length: some_length (length of the copy string)
     */
    printf("Duplicated string: %s\n", copy);
    printf("Length: %ld\n", my_strlen(copy));

    /* 
     * 4. Deallocate (free) any memory allocated with malloc/calloc/realloc/etc.
     */
    free(copy);

    // REMEMBER TO CHECK VALGRIND FOR THIS. NO MEMORY DIARRHEA!!!

    return EXIT_SUCCESS;
}

/**
 * This function calculates the length of a character array passed in,
 * returning the length as a size_t. Calculation must be done via pointer
 * arithmetic. No credit will be given for calling strlen() in <string.h>.
 *
 * src: character array, possibly NULL (which will be considered length 0)
 *
 * return: length of src as a size_t
 */
size_t my_strlen(char *src) {
    /* TODO: Implement me! */
    if (src == NULL) {
        //printf("null src \n");
        return 0;
    } 
    size_t len = 0;
    // Iterate until we hit the terminating character.
    char* t = src;
    while (*t) {
        len++;   // Increment char. count
        t++;   // Move to next character
    }
    //printf("length %ld \n", len);
    return len;
}

/**
 * This function copies a character array src into dest. The caller must ensure
 * that the length of dst is at least length of src plus one. This function then
 * returns a pointer to dst. Computation must be done via pointer arithmetic.
 * No credit will be given for calling memcpy() in <string.h>.
 *
 * dst: target character array
 * src: source character array, possibly NULL
 *
 * return: pointer to dst
 *         If src is NULL, return NULL.
 */
char *my_strcpy(char *dst, char *src) {
    /* TODO: Implement me! */
    //source is empty
    if (!src) {
        return NULL;
    }
    // I think this is quite similar to the string.c code, but let's talk about it.
    /* while loop to copy each elem of source into dst, so create another pointer
     * pointing at dest, then increment it through and return destination, just like in mystrlen */
    char* t = dst;
    char* u = src;
    while(*u) {
        *t = *u;
        t++;
        u++;
    }
    *t = '\0';
    return dst;
}
