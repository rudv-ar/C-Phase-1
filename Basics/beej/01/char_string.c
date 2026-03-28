// this is how we store the non array strings, or char pointers.
// as I already covered in the memory section, every string which we pass : "hello", "whatever" "some 200 char long", are all stored in the .rodata
// .rodata means it is a readonly region : r__ hex permission. So we cannot change their value. But we can do the change in case of array string.
#include <stdio.h>

int main(void) {
  char *hello = "Hello World!";
  // try getting the address of this stuff : &hello 
  printf("Address of - %s : %p\n", hello, (void *)hello); 
  printf("Address of pointer hello - %p\n", (void *)&hello);
  // &hello - give me the address of the variable(pointer type) hello : which is a pointer.

}
