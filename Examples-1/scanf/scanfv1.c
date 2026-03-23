// We are goint to get user input ! Interesting.
// scanf : this is a functoin that is found in the standard input ouput lib. Users can provide NON SPACE SEPERATED INPUT. 

#include <stdio.h> 

int main(void) {

  int a;

  printf("Enter a number : "); // I am not using \n for purpose of input visuals..
  scanf("%d", &a); // referencing the undeclared int value a, I need to get the user input and store it in that value. 
                  // This stuff is similar to the borrowing of a variable in rust, expecpt that here we call it using the actual name : reference
  printf("You entered this number groot : %d\n", a);
  // Now we have got different cases to try : 
  // - What happens if a is not initialised in the first place? 
  // - What happens if &a is not provided?
  // - What happens if scanf part was commented? -- obvious from the Basics of C Phase 1 stuff, not example dir, we know it produces undefined behaviour. In that example using an uninitialsed variable produced zero as an output. This time it may or may not be sero...
  // - providing two numbers instead of one seperated by space 
  // - providing a non integer instead of integer
  // - providing a negative number instead of positirve
  // - providing a negative and positirve float resp
  // - providing this stupid number 4248792837458972938579238472937538927323423 - probably it is an integer overflow error
  // - providing nothing
  return 0;
                  
}
