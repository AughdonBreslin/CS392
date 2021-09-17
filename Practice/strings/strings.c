#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Copying a string using memcpy
char* my_strdup_mem(const char *s) {
    // Anytime a pointer is taken in, check if it is null:
    if (!s) {
        return NULL;
    } 
    size_t len = strlen(s) + 1; // Include space for null terminating char, '\0'.

    // How many characters * how big each one of that type is.
    // A good idea when you're using malloc; also a good idea to cast.
    // We are allocating a certain number of bytes.
    char *copy = (char *)malloc(len * sizeof(char));
    if (!copy) {
        return NULL;
    }
    return (char *)memcpy(copy, s, len);
}

// Copying a string with pointer arithmetic
char* my_strdup_ptr(const char *s) {
    // First few lines are exactly like the above function.
    if (!s) {
        return NULL;
    } 
    size_t len = strlen(s) + 1;
    char *copy = (char *)malloc(len * sizeof(char));
    if (!copy) {
        return NULL;
    }

    // In using pointer arithmetic, we want to loop over string array.
    // t will point to the same memory copy is pointing to.
    char *t = copy;
    // We want to dereference each character in s
    while (*s) {
        *t++ = *s++; // Real pro coding :)
        /**
         * This is the condensed version of:
         * *t = *s;
         * t++;
         * s++;
         */
    }
    *t = '\0'; // This makes the last character in the string.

    return copy;
}

int main() {
    char name[128];                // Declare a character array of size 128.
    printf("What is your name? "); // Prompt user, "What is your name?"
    // Read input into name. By default, name points to the first address in array.
    // NOT SAFE: if your input is >128 characters, STACK SMASHING!
    scanf("%s", name);      

    printf("Hello, %s!\n", name);
    /**
     * According to man strlen: the function calculates the length of the string
     * pointed to by s, excluding the terminating character '\0'
     * 
     * Using %ld because size_t will return an unsigned long int
     */
    printf("Your name has %ld characters.\n", strlen(name));

    // memcpy way
    char *copy1 = my_strdup_mem(name);
    printf("Hello, %s!\n", copy1);
    printf("Your name has %ld characters.\n", strlen(copy1));

    // pointer arithmetic way
    char *copy2 = my_strdup_ptr(name);
    printf("Hello, %s!\n", copy2);
    printf("Your name has %ld characters.\n", strlen(copy2));

    printf("How old are you? ");
    int age;
    scanf("%d", &age); // age is not an array, so we need & to give address of variable.
    printf("You are %d year(s) old.\n", age);

    printf("How old will you be in 1 year? ");
    scanf("%d", &age);
    printf("You will be %d year(s) old next year.\n", age);

    // At the end, you must free up allocated memory at the end!
    // Check with valgrind --leak-check=yes ./<program>
    // MAKE VALGRIND HAPPY
    free(copy1);
    free(copy2);

    return 0;
}