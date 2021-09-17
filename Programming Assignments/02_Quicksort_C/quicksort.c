/*******************************************************************************
 * Name        : quicksort.c
 * Author      : Matthew Oyales & Audie Breslin
 * Date        : 5 June 2021
 * Description : Quicksort implementation.
 * Pledge      : I pledge my honor that I have abided by the Stevens Honor System.
 ******************************************************************************/
#include <stdio.h>
#include <string.h>
#include "quicksort.h"

/* Static (private to this file) function prototypes. */
static void swap(void *a, void *b, size_t size);
static int lomuto(void *array, int left, int right, size_t elem_sz,
                  int (*comp) (const void*, const void*));
static void quicksort_helper(void *array, int left, int right, size_t elem_sz,
                             int (*comp) (const void*, const void*));

/**
 * Compares two integers passed in as void pointers and returns an integer
 * representing their ordering.
 * First casts the void pointers to int pointers.
 * Returns:
 * -- 0 if the integers are equal
 * -- a positive number if the first integer is greater
 * -- a negative if the second integer is greater
 */
int int_cmp(const void *a, const void *b) {
    // TODO

    // Cast void pointers to int pointers
    int alpha = *((int*)a);
    int beta = *((int*)b);

    //Returns
    if(alpha > beta) {
        return 1;
    } else if (alpha < beta) {
        return -1;
    }
    return 0;
}

/**
 * Compares two doubles passed in as void pointers and returns an integer
 * representing their ordering.
 * First casts the void pointers to double pointers.
 * Returns:
 * -- 0 if the doubles are equal
 * -- 1 if the first double is greater
 * -- -1 if the second double is greater
 */
int dbl_cmp(const void *a, const void *b) {
    // TODO
        
    // Cast void pointers to double pointers
    double alpha = *((double*)a);
    double beta = *((double*)b);

    //Returns
    if (alpha > beta) {
        return 1;
    } else if (alpha < beta) {
        return -1;
    }
    return 0;
}

/**
 * Compares two char arrays passed in as void pointers and returns an integer
 * representing their ordering.
 * First casts the void pointers to char* pointers (i.e. char**).
 * Returns the result of calling strcmp on them.
 */
int str_cmp(const void *a, const void *b) { 
    // TODO

    // Cast void pointers to char* pointers
    char *alpha = *((char**)a);
    char *beta = *((char**)b);

    //Returns
    return strcmp(alpha,beta);
}

/**
 * Swaps the values in two pointers.
 *
 * Casts the void pointers to character types and works with them as char
 * pointers for the remainder of the function.
 * Swaps one byte at a time, until all 'size' bytes have been swapped.
 * For example, if ints are passed in, size will be 4. Therefore, this function
 * swaps 4 bytes in a and b character pointers.
 */
static void swap(void *a, void *b, size_t size) {
    // TODO

    // Cast void pointers to char* pointers
    char *alpha = (char *) a;
    char *beta = (char *) b;

    // Before I touch this, should this be using the pointer form?
    for (int i = 0; i < size; i++) {
        char temp = alpha[i];
        alpha[i] = beta[i];
        beta[i] = temp;
    }
}

/**
 * Partitions array around a pivot, utilizing the swap function.
 * Each time the function runs, the pivot is placed into the correct index of
 * the array in sorted order. All elements less than the pivot should
 * be to its left, and all elements greater than or equal to the pivot should be
 * to its right.
 * The function pointer is dereferenced when it is used.
 * Indexing into void *array does not work. All work must be performed with
 * pointer arithmetic.
 */
static int lomuto(void *array, int left, int right, size_t elem_sz,
                  int (*comp) (const void*, const void*)) {
    // TODO
    /* NOTE: check the pointer arithmetic (PA) because im not positive
    im doing it correctly */
    // It has to be char* because until the function is called with the correct parameters,
    // we have no idea what type we want to sort by, so it's a void pointer for now.
    // Think of a void pointer as a form of abstraction away from the specific type.
    char *arr = (char *) array;
    char *p = arr + (left * elem_sz);            // PA
	int s = left;
    // Any time we work with the index, we have to make sure we are incrementing by the right amount, depending on type.
	for(size_t i = left+1; i <= right; i++){
		if(comp((arr+(i * elem_sz)), p) < 0){    // PA
			s++;
			swap(arr+(s * elem_sz), arr+(i * elem_sz), elem_sz);  // PA
		}
	}
	swap(arr+(left * elem_sz), arr+(s * elem_sz), elem_sz);       // PA
    return s;
}

/**
 * Sorts with lomuto partitioning, with recursive calls on each side of the
 * pivot.
 * This is the function that does the work, since it takes in both left and
 * right index values.
 */
static void quicksort_helper(void *array, int left, int right, size_t elem_sz,
                             int (*comp) (const void*, const void*)) {
    // TODO
    //only concern is casting to int
    // int* arr = (int*)array; no casting needed
    if(left < right) {
        int s = lomuto(array, left, right, elem_sz, comp);
        
        quicksort_helper(array, left, s-1, elem_sz, comp);
        quicksort_helper(array, s+1, right, elem_sz, comp);
    }
}

/**
 * Quicksort function exposed to the user.
 * Calls quicksort_helper with left = 0 and right = len - 1.
 */
void quicksort(void *array, size_t len, size_t elem_sz,
               int (*comp) (const void*, const void*)) {
    // TODO
    int left = 0;
    int right = len - 1;
    quicksort_helper(array, left, right, elem_sz, *comp);
}
