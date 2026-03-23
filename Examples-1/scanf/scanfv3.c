// We are goint to get user input ! Interesting.
// scanf : this is a functoin that is found in the standard input ouput lib. Users can provide NON SPACE SEPERATED INPUT. 

#include <stdio.h> 

int main(void) {

  int a;

  printf("Enter a number : "); // I am not using \n for purpose of input visuals..
  scanf("%d"); // referencing the undeclared int value a, I need to get the user input and store it in that value. 
                  // This stuff is similar to the borrowing of a variable in rust, expecpt that here we call it using the actual name : reference
  printf("You entered this number groot : %d\n", a);
  return 0;
                  
}
