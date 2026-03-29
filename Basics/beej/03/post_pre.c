// the Post and Pre increment or decrements...
// it is just self instructing : ++i means first increment i, then return i 
// i++ means : first return i, then increment i 
// --i : first decrement, then return i 
// i-- : first return, then decrement i
#include <stdio.h>

int main(void) {
    int x = 99;
    printf("Before prefix increment x : %d\n", x);
    int y = ++x; 
    // increment x, x = 100, return x, y = 100.
    printf("After prefix increment x : %d\n", x);
    printf("After prefix increment of x ; value of y = ++x : %d\n", y);
    int z = 100 + ++x;
    // increment x again : 101 now, return x, 100 + 101 = 201; this is value of z.
    printf("After the second prefix increment of x , value of z : %d\n", z);
    z = ++x - ++y;
    // increment x again, 102 now, return x, increment y, now y is 101. subtract them both.
    printf("The value of z = x - y = %d - %d = %d\n", x, y, z);
}
