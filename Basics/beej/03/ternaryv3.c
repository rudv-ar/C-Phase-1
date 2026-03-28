// these are something which I saw in the QML bool based property setting. color: width > 100 ? 10 : 20 
// This statement will  assign to color if width is greater than 100, it gives 10, else 20.
// These are just if else statements in expressions...

#include <stdio.h>
#include <string.h>
// this is another new library I am encountering today : in one day, we have come across stdint.h, stdbool.h, string.h : three new libs..

int main(void) {

    int parent_width = 200; // I am stack here!!...
    char *parent_color = "green"; // this is pointer to a address in .rodata

    printf("This is width : %d; This is color : %s", parent_width > 100 ? 100 : parent_width, strcmp(parent_color,"black") == 0 ? "white" : parent_color);
    // for now just remember, strcmp involves asci number comparision : small > caps, see the error dir for more explanation. right compared with left...
    // "Hello", "hello" will be less than zero, as "hello" > "Hello"
    // if parent width is greater than 100, then print width as 100, else print the same as parent width; 
    // if the parent color is eqall to black, then print white for color, else same as parent_color
}
