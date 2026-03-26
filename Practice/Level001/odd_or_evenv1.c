// I could possible think of only one version for this odd even stuff friends. If you have any other version, git pull it.

// as usuall process : include stuffs and int main(void : in case of no args...)

#include <stdio.h> 

int main(void) {
  // initialise a zero int 
  int a = 0;
  printf("Enter a number to check for odd or even : ");
  scanf("%d", &a);
  if (a % 2 == 0) {
    printf("\nThe number %d is even.\n", a);
  } else {
    printf("\nThe number %d is odd\n", a);
  }
}
