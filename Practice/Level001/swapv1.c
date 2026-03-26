// here we are going to take two integer inputs, print them swapped. There are obviously different ways to swap the integers.
// Here I am going to use an intermediate temproary variable for exec the swap 
// scanf for taking integer input 


// include the header : standard input output lib 
#include <stdio.h> // #include<stdio.h> is also correct; gcc does not care about spaces.

// define the main function 

int main(void) {
  // this main returns an integer type, takes zero arguments : void 
  // initialise the empty integers, possibly setting them to 0 is a good behaviour.
  int a = 0, b = 0;
  printf("Enter two numbers seperated by a single space, make sure the numbers are integers : ");
  // I am not adding a newline here for visual purposes.
  // scanf takes two integers
  scanf("%d %d", &a, &b);
  // Note that we are using a reference : we are passing the address of the variables here, not the variables themself.
  // If you are somehow able to see how scanf is defined, it is defined to take input of an mem address pointing to the variable (a, b in this case)
  // Then it uses dereference to modify the values in that address we have just passed : *a = .. *b = ...
  // Done.
  printf("\nThe variables value (a,b) = (%d, %d) \n", a, b);
  
  // perform the swap 
  int temp = a;
  a = b;
  b = temp;

  printf("The swapped variables (a, b) = (%d, %d) \n", a, b);
}
