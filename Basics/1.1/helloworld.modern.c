// This is the comment in C 

#include <stdio.h> 
// This is the standard input output library, here exists a function named printf from which we will be able output to the standard output

int main(void) { // without this void, the C understands that any number of arguments can be passed. That is dangerous.
    printf("Hello World!\n");
    printf("Ah, the escape characters...!\n");
}

// What, wait? what about the return code? The above code still executes? Why is that executing? Huh. See the helloworld.another.c
// With void passed into the main() -> this implies that function main explicitly accepts zero arguments.
