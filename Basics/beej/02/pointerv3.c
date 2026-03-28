#include <stdio.h>

int main(void) {
    int a = 100; // as we know, this is an integer type
    float b = 3.14;
    char c = 'A';
    double k = 3.234234;
    long l = 234234L; // long integer type.. (wait... is that L suffix necessary? try removing in v4...)
    unsigned int alpha = 2323;

    // pointers : <type> *
    int *p1 = &a; 
    // remember that &a returns us int * type . &a : will return an address : that address will be of the type int * => So every int *p and &a provided a is int type will have the same type : int * which is nothing but a pointer. But this does not mean that &a is also a pointer as they have the same type (atleast in my opinion, I don't know how universal people think about this...)
    float *p2 = &b;
    // the comment under int *p1 = &a applies here too. So if you are my future version, you would know how to navigate the bias...
    char *p3 = &c;
    double *p4 = &k;
    long *p5 = &l;
    unsigned int *p6 = &alpha;

    // Now a question arises out of nowhere : why does not C implicitly convert int *p to void *p in a variadic function?
    // What? a variadic function? As per the definition of the function : it takes any number of arguments, and does not gaurentee the type of those arguments... : Exactly the example is printf(char *s, *fmt, ...) : ... means anything that comes after this stuff. 
    // The formatter %p does not know what the hell type p1, p2 are those stuffs are, ie it does not know wheter the pointer is int *, char *, or something.
    // The modern systems have the sizes of all these pointer types same... But wait!!!! Why is the C standard not gaurenting the same size for all pointer types? Mystery for me. Maybe I will be able to decode it later. Currently, even my old system is able to output an expected result without type casting the pointers thou C STANDARD MENTIONS IT AS UB. So, as per my opinion, there SHOULD EXIST ATLEAST ONE SYSTEM IN THIS WORLD WHERE WE CAN SEE AN UNDEFINED BEHAVIOUR.

    printf("a: type int :%d - address : %p\n", a, (void *)p1); 
    // Ah, I found it... (Maybe true) : Since C is a old relic language that seems to be appeared out of assembly stuff, many people tried standardising this C language so badd...
    // I consider too many standardisations overwhelming, but yet : That is just a version update they call "Standards?"; Well known fact that they add features and fix bugs at each standard.
    // Now my question is : why should we even bother to cast a pointer? Just now, I learnt that : void * is a generic pointer. 
    printf("b: type float :%f - address : %p\n", b, (void *)p2);
    printf("c: type char :%c - address : %p\n", c, (void *)p3);
    printf("k: type double :%f - address : %p\n", k, (void *)p4);
    printf("l: type long :%ld - address : %p\n", l, (void *)p5);
    printf("alpha: type unsigned int :%d - address : %p\n", alpha, (void *)p6);



}
