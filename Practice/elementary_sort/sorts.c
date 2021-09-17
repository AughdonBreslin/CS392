#include <stdio.h>
#include <stdlib.h>
#include "sorts.h"
// This file has the implementation of the functions in sorts.h

/** Convert the 385 code to C code:
* static void swap(int array[], const int a, const int b) {
*    int temp = array[a];
*    array[a] = array[b];
*    array[b] = temp;
*}
*
*void bubble_sort(int array[], const int length) {
*    int len = length;
*    while (len > 0) {
*        int n = 0;
*        for (int i = 1; i < len; ++i) {
*            if (array[i - 1] > array[i]) {
*                swap(array, i - 1, i);
*                n = i;
*            }
*        }
*        len = n;
*    }
*}
*/
static void swap(int *array, const int a, const int b) {
    int temp = *(array + a);
    *(array + a) = *(array + b);
    *(array + b) = temp;
}

void bubble_sort(int *array, const int length) {
    int len = length;
    while (len > 0) {
        int n = 0;
        for (int i = 1; i < len; ++i) {
            if (*(array + i -1) > *(array + i)) {
                swap(array, i-1, i);
            }
        }
        len = n;
    }
}

// Recreating code from Elementary Sorts module, for selection sort.
void selection_sort(int *array, const int length) {
    for (int i = 0, i_bound = length - 1; i < i_bound; ++i) {
        int min_index = i, min = *(array + i);
        for (int j = i + 1; j < length; ++j) {
            if (*(array + j) < min) {
                min_index = j;
                min = *(array + j);
            }
        }
        if (min_index != i) {
            swap(array, i, min_index);
        }
    }
}

void insertion_sort(int *array, const int length) {
    for (int i = 1; i < length; ++i) {
        int k, current = *(array + i); // Same as saying current = array[i]
        for (k = i - 1; k >= 0 && *(array + k) > current; --k) {
            *(array + k + 1) = *(array + k);
        }
        *(array + k + 1) = current;
    }
}

// Display in Python format!
void display_array(int *array, const int length) {
    putchar('[');
    if (length > 0) {
        printf("%d", *array); // First element of the array. array[0]
    }
    for (int i = 1; i < length; i++) {
        printf(", %d", *(array + i));
    }
    puts("]");
}
