#ifndef SORTS_H_
#define SORTS_H_
// If the header file is not defined, then we make it.
// Prevents having to recompile multiple times, avoiding circular dependencies.

/* Function prototypes */
void bubble_sort(int *array, const int length);
void selection_sort(int *array, const int length);
void insertion_sort(int *array, const int length);
void display_array(int *array, const int length);

#endif
