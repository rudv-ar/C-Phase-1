// but we can include the <stdbool.h> lib to get the benefits of modern C with the bool type supported...

#include <stdio.h>
// let's now comment this out... #include <stdbool.h> // but this is not needed in c23 itseems, it is included by default. 
                     // and my bad, by v1 error appeared because I compiled with -std=c23.
                     // not a problem, we know....c23 does not allow it. (as in v1)
                     // by default <stdbool.h> is included by default by <stdio.h> 


const bool TRUE = true;
const bool FALSE = false;
int main(void) {

    if (TRUE) {
        printf("This is output of true statement;\n");
    } if (! FALSE) {
        printf("This is the output of false statement;\n");
    }
}

