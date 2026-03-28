// historically, c did not have any bool type. In C, 0 is false, any other integer is true, includes negative or positive.
// we can demonstrate a simple bool conditioned stuff using if else statements.

#include <stdio.h> 

int main(void) {
    int is_true = 1;
    int is_false = 0;

    if (is_true) {
        printf("This will be printed if true...\n");
    }
    
    if (is_false) {
        printf("This will be not be printed if false...\n");
    }
}
