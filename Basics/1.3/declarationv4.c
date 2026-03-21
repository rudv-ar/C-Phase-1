// let me try initialising c variables in rust way, will it work? the K and R does not mention this. 

#include <stdio.h>

int main(void) {
    int v = 100;
    int u = 300; // After going through the teditous process of variable declaration I realised a fact.
    int z = 200; // Added this semicolon after the LSP instruction... let's check this, what happens?
    printf("%d\t%d\t%d\n", v, u, z);


}

// is there a good way to declare variables? 
// int v, u, z; v = 100; u = 300; z = 200; -> This syntax is valid if I am not wrong. Gonna check this in v5.
// ve use int only one time... But in case of declaration + initialisation, we have used int for 3 times, 6 words extra. Overall, using this method, we are in need of typing additional 3 more letters. Huh.
