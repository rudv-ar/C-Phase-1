// it happens to be I accidently discovered a more safer alternative for printf - the formating features (- = minus)
// puts....
// I noticed this puts in a disassembly. When you do not use any of the formating features of printf like %c to insert a char stuffs like that, 
// The compiler will optimise that for a SAFER AND FASTER purpose....
// I will replace the printf with puts..... and this puts is available in the stdio.h lib.

#include <stdio.h> 

int main(void) {
    printf("this is a printf");
    // use the objdump -x to get the output of this as header files, -d for dumping the disassembly : lets go with -d...

}
