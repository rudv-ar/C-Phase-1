// this is swap using arithmetics ; continued from v1, see v1, come here.

#include <stdio.h> 

int main(void) {
  int a = 0, b = 0;
  printf("Enter two numbers as integers seperated by space : ");
  scanf("%d %d", &a, &b);
  printf("\nBefore swap : (a, b) = (%d, %d)\n", a, b);

  // swap process :
  // a = initial a = entered value 
  // b = initial b = entered value 
  a = a + b; // = initial a + initial b
  b = a - b; // = a - initial b = (initial a + initial b) - (initial b) = initial a 
  a = a - b; // = (initial a + initial b) - (initial a) = initial b 
  // We have just exec a swap using just arithmetics ; BUT, remember what will happen if you go with such a number ? 238974928374986923598729385723989823749987234230982039598723598723095873284798273498729384729834792834729835729385729837498273958702837428934520398524730973097852378456856547457845764578 : OBVIOUSLY, No one is going to enter such a big number for a simple swap, but still this : even long long a = 0; is not going to protect you from this long number, using %lld format specifier.
  // Horribly landing into an integer overflow errow : which the gcc may completely ignore, fail silently or crash in production code.
  printf("Swapped using arithmetics : (a, b) = (%d, %d)\n", a, b);
  return 0;
  

}
