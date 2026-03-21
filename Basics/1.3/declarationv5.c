// let me try initialising c variables in rust way, will it work? the K and R does not mention this. 

#include <stdio.h>
// What is the purpose of this include stuff? this is a c preprocessor, which just copies that stuff in stdio.h from default lib location into this file in the static memory during the compilation. <...> means we include from default lib dir, that is root stuff, "..." means we ask the preprocessor to first search in this current directory before proceeding..

// What happens without this preprocessor include? Will printf not be available? Check in v6

int main(void) { int v = 100; int u = 300; int z = 200; printf("%d\t%d\t%d\n", v, u, z);}

// Ah, the valid and ugly syntax. I mean this is not a syntax anymore. Just raw instructions seperated inside a function by `;` just like precious bash scripts. C is essentially a bash script! (just kidding.); But tbh, even bash has a syntax, but here this seems something easy? Write C, need to syntax?
