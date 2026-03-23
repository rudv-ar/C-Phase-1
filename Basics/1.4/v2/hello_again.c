#include "stdio.c" 

int main(void) {
  try_func();
  printf("This is the real hello world...\n");

} // What about renaming this header to stdio.c? what happens?
  // try that in v2 : Ok, it works, so it : anything.h is just a representation that it is not a normal c file, but a headers file which does not enclude any executable stuffs, but only definitions and declarations of Constants andthose functions are actually present in another file. libc libraray.
