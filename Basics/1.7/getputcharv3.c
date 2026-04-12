// the get and put chars 

#include <stdio.h> 

int main(void) {
    int c = 0;
    c = getchar(); // try giving it a number this time?
    putchar(c);
    int a = -155;
    putchar(a); // it will print A; why? because unicode 65 is A.
    // that's the matter with it all. 
    
    // what happens if we give arbitarily large nummber like this? 21342314?

}
