#include <stdio.h>

void dirty_stack() {
    volatile char junk[5] = {11, 22, 33, 44, 55};
    (void)junk;
}

int main(void) {
    dirty_stack();
    char arr[5];
    printf("%d\n", (int)arr[0]);
}
