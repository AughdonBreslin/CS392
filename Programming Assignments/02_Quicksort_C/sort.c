/*******************************************************************************
 * Name        : sort.c
 * Author      : Matthew Oyales & Audie Breslin
 * Date        : 5 June 2021
 * Description : Uses quicksort to sort a file of either ints, doubles, or
 *               strings.
 * Pledge      : I pledge my honor that I have abided by the Stevens Honor System.
 ******************************************************************************/
#include <errno.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "quicksort.h"
//gnu libs
#include <ctype.h>
#include <unistd.h>

#define MAX_STRLEN 64     // Not including '\0'
#define MAX_ELEMENTS 1024

typedef enum
{
    STRING,
    INT,
    DOUBLE
} elem_t;

/**
 * Basic structure of sort.c:
 *
 * Parses args with getopt.
 * Opens input file for reading.
 * Allocates space in a char** for at least MAX_ELEMENTS strings to be stored,
 * where MAX_ELEMENTS is 1024.
 * Reads in the file
 * - For each line, allocates space in each index of the char** to store the
 *   line.
 * Closes the file, after reading in all the lines.
 * Calls quicksort based on type (int, double, string) supplied on the command
 * line.
 * Frees all data.
 * Ensures there are no memory leaks with valgrind. 
 */

/**
 * TEMPLATE FROM THE ASSIGNMENT
 * Reads data from filename into an already allocated 2D array of chars.
 * Exits the entire program if the file cannot be opened.
 */
size_t read_data(char *filename, char **data) {
    // Open the file.
    FILE *fp = fopen(filename, "r");
    // CASE 3 -- Invalid filename
    if (fp == NULL) {
        fprintf(stderr, "Error: Cannot open '%s'. %s.\n", filename,
                strerror(errno));
        free(data);
        exit(EXIT_FAILURE);
    }

    // Read in the data.
    size_t index = 0;
    char str[MAX_STRLEN + 2];
    char *eoln;
    while (fgets(str, MAX_STRLEN + 2, fp) != NULL) {
        eoln = strchr(str, '\n');
        if (eoln == NULL) {
            str[MAX_STRLEN] = '\0';
        } else {
            *eoln = '\0';
        }
        // Ignore blank lines.
        if (strlen(str) != 0) {
            data[index] = (char *)malloc((MAX_STRLEN + 1) * sizeof(char));
            strcpy(data[index++], str);
        }
    }

    // Close the file before returning from the function.
    fclose(fp);

    return index; // count of how many lines.
}

/**
 * Function to print out the usage message using multiline printf.
 * SOURCE: https://stackoverflow.com/questions/1752079/in-c-can-a-long-printf-statement-be-broken-up-into-multiple-lines
 */
void print_usage(char *progname) {
    printf
    ("Usage: %s [-i|-d] filename\n"
     "   -i: Specifies the file contains ints.\n"
     "   -d: Specifies the file contains doubles.\n"
     "   filename: The file to sort.\n"
     "   No flags defaults to sorting strings.\n", progname);
}

int main(int argc, char **argv) {

    // CASE 1 -- No input arguments
    if (argc == 1) {
        print_usage(argv[0]);
        return EXIT_SUCCESS;
    }

    int intflag = 0;
    int doubflag = 0;
    int stringflag = 0;
    int c;

    // Setting opterr to 0 will supress the default error.
    opterr = 0;

    while ((c = getopt(argc, argv, "id")) != -1) {
        switch (c) {
        case 'i':
            intflag = 1;
            break;
        case 'd':
            doubflag = 1;
            break;
        case '?':
            //NOTE: Not sure what this if and the else are used for
            if (optopt == 'c')
                fprintf(stderr, "Option -%c requires an argument.\n", optopt);
            else if (isprint(optopt)) {
                // CASE 2 (AND CASE 7)
                fprintf(stderr, "Error: Unknown option '-%c' received.\n", optopt);
                print_usage(argv[0]);
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
    // bodge solution (LOL)
    if (intflag + doubflag == 0) {
        stringflag = 1;
    }

    // Handy debug line.
    /*printf("intflag = %d, doubflag = %d, stringflag = %d \n",
           intflag, doubflag, stringflag);*/

    // CASE 6 -- Multiple Valid Flags
    /* NOTE: I edited it because i thought it looks cool this way
         doesnt actually add value */
    if (intflag + doubflag + stringflag > 1) {
        printf("Error: Too many flags specified.\n");
        return EXIT_FAILURE;
    }
    
    // CASE 4 -- No filename
    // If you give a flag but don't give a file after the flag:
    // NOTE: Not possible for stringflag. It would be "./sort  ", or CASE 1
    if ((intflag || doubflag) && argc == 2) {
        printf("Error: No input file specified.\n");
        return EXIT_FAILURE;
    }

    // CASE 5 -- Multiple filenames
    if (argc > (3 - stringflag)) {
        printf("Error: Too many files specified.\n");
        return EXIT_FAILURE;
    }

    /** The contents of the input files will be correct. 
     * You may safely assume that each line contains a string, int, or double, 
     * and that every line in the file will be of the same data type.
     * There will be at most 1024 lines in a file, and each line will contain up to 64 printable (visible) characters. 
     * Every line in the file, including the last line, will end in a newline character. 
     */

    size_t size;
    char **arr = (char **)malloc(sizeof(char *) * MAX_ELEMENTS);
    size = read_data(argv[2-stringflag], arr);

    // Somewhere here, call quicksort based on type.
    if (intflag) {
        // Do quicksort on ints.
        // HINT: look at atoi; we need to do this before sorting.
        // SOURCE: https://www.tutorialspoint.com/c_standard_library/c_function_atoi.htm
        // This should sort out the string representation of the number and make it an int.
        int int_arr[size];
        for (int i = 0; i < size; i++) {
            int_arr[i] = atoi(arr[i]);
        }

        quicksort(int_arr, size, sizeof(int), int_cmp);

        for (int i = 0; i < size; i++) {
            printf("%d\n", int_arr[i]);
        }
    } else if (doubflag) {
        // Do quicksort on doubles.
        // HINT: look at atof
        // SOURCE: https://www.tutorialspoint.com/c_standard_library/c_function_atof.htm
        double doub_arr[size];
        for (int i = 0; i < size; i++) {
            doub_arr[i] = atof(arr[i]);
        }

        quicksort(doub_arr, size, sizeof(double), dbl_cmp);

        for (int i = 0; i < size; i++) {
            printf("%0.6f\n", doub_arr[i]);
        }
    } else {
        // Quicksort on strings is the only other possibility.
        quicksort(arr, size, sizeof(char *), str_cmp);

        for (int i = 0; i < size; i++) {
            printf("%s\n", arr[i]);
        }
    }

    /*
     * Have this note for later: 
     * In order to force double sorted output to have exactly six
     * levels of precision, you can use printf("%0.6f", num)
     * 
     * https://www.geeksforgeeks.org/g-fact-41-setting-decimal-precision-in-c/
     */
    
    // Remember before program ends to LIBERATE THE MEMORY!
    // DO NOT GIVE VALGRIND MEMORY DIARRHEA!
    for (int i = 0; i < size; i++) {
        free(arr[i]); // Free each element of the array
    }
    free(arr);        // Free the container

    return EXIT_SUCCESS;
}
