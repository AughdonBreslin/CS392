#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MIN(x, y) (((x) < (y)) ? (x) : (y))

typedef union my_union {
    char c;
    short s;
    int i;
} my_union_t;

/**
 * Displays the values of the union's members.
 */
void display(my_union_t u) {
    printf("%c\n", u.c);
    printf("%d\n", u.s);
    printf("%d\n", u.i);
}

/**
 * Displays a line with + symbols at 8-bit markers.
 */
void display_line(unsigned int num_bytes) {
    putchar('+');  // putchar() allows you to put one character on the screen.
    for (int i = num_bytes * 8 - 1; i >= 0; i--) { // Comparing against 0 in a loop is faster!
        putchar('-');
        // Check if we are at an 8 bit marker
        if ((i % 8) == 0) {
            putchar('+');
        }
    }
    putchar('\n');
}

/**
 * Converts the union into an unsigned int and performs bitwise operations
 * to display the contents of memory occupied by the union.
 */
void display_bits(my_union_t u) {
    unsigned int
        p,
        num_bytes = MIN(sizeof(my_union_t), sizeof(unsigned int)); // whichever is smaller is what will be displayed.
    // IMPORTANT: we are copying to the ADDRESS of p, which is why we use '&p'
    memcpy(&p, &u, num_bytes); // Copies num_bytes amount of bytes from address of u to p.
    display_line(num_bytes);   // ASCII art line, as defined above.
    putchar('|');              // Put a pipe at every 8 bits
    unsigned int mask;
    for (int i = num_bytes * 8 - 1; i >= 0; i--) {
        mask = 1 << i;         // Shift mask by i places
        if (p & mask) {        // Bitwise & checks if particular digit to print is 0 or 1.
            putchar('1');
        } else {
            putchar('0');
        }
        if ((i % 8) == 0) {    // If we get to the end of an 8-bit block:
            putchar('|');      // Separate with a '|' character
        }
    }
    putchar('\n');
    display_line(num_bytes);
}

int main() {
    my_union_t u;  // Sitting on the stack
    int var0;

    u.i = 0;
    display(u);

    u.c = 'A';
    display(u);

    u.s = 16383;
    display(u);

    display_bits(u);

    var0 = u.c;         // We will only see the lowest order 8 bits
    printf("%d\n", u.c);
    printf("%d\n", var0);
    u.i = var0;

    display_bits(u);    // All 1s, because representation of -1 in 32 bits.

    return 0;
}