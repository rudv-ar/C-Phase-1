#include <stdio.h>

int main(void) {
    int u, l, s, c, f;
    // the compiler does not explicitly tell that I missed this semicolon; but result? they simply became undeclared and broke the remaining stuff.
    // Declaraition part here.
    u = 1000;
    l = 0;
    s = 10;
    f = l;

    // the while loop
    while (f<=u) {
        c = (5/9) * (f - 32);
        printf("%d\t%d\n", f, c); // If I never provided arguments, see what happens
        f = f + s;
    }

    return 0;

}

// this code is perfect. But wait, let me try some bodmas..
