// a program which converts the input to output one character at a time.
// include the standard input outpu library 

#include <stdio.h> 

// define the main function 
int main(void) {
    // initialse a empty integer. 
    int c = 0; // initialse an interger named c.
    // while until the character is equal to EOF. 
    while (c != EOF) { // we are not use char type for c because char is only one byte, wheras the EOF is more than that, it is an int type, which requires more size enough to hold that value before overflowing.
        c = getchar(); // getchar gets one character at a time and stored in the c variable. wait, but c is an int type, how can it store char? Well it gets converted into its own unicode value, which is unsigned integer value.
        putchar(c); // it prints that specific character.
    }
}
