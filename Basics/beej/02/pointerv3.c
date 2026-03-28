#include <stdio.h>

int main(void) {
    int a = 100;
    float b = 3.14;
    char c = 'A';
    double k = 3.234234;
    long l = 234234L;
    unsigned int alpha = 2323;

    // pointers : <type> *
    int *p1 = &a; // remember that &a returns us int * type .
    float *p2 = &b;
    char *p3 = &c;
    double *p4 = &k;
    long *p5 = &l;
    unsigned int *p6 = &alpha;

    printf("a: type int :%d - address : %p\n", a, (void *)p1); // we should note at this point that p1 = &a both will be producing the same address. Did we fail to cast the pointer?
    printf("b: type float :%f - address : %p\n", b, (void *)p2);
    printf("c: type char :%c - address : %p\n", c, (void *)p3);
    printf("k: type double :%f - address : %p\n", k, (void *)p4);
    printf("l: type long :%ld - address : %p\n", l, (void *)p5);
    printf("alpha: type unsigned int :%d - address : %p\n", alpha, (void *)p6);



}
