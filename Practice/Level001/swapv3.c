// XOR swap : continued from v1 (temp), v2 (arithmetic). See those first.
// We are using the XOR bitwise operator to swap without a temp variable.
// XOR truth : a ^ a = 0 (anything XOR itself is 0)
//             a ^ 0 = a (anything XOR 0 is itself)
// These two rules are all you need to trace why this works.

#include <stdio.h>

int main(void) {
  int a = 0, b = 0;
  printf("Enter two integers separated by a space : ");
  scanf("%d %d", &a, &b);
  printf("\nBefore swap : (a, b) = (%d, %d)\n", a, b);

  // XOR swap :
  a = a ^ b; // a now holds a ^ b
  b = a ^ b; // = (a ^ b) ^ b = a ^ (b ^ b) = a ^ 0 = a : b is now initial a
  a = a ^ b; // = (a ^ b) ^ a = b ^ (a ^ a) = b ^ 0 = b : a is now initial b
  // No temp, no arithmetic, no overflow risk.
  // BUT : never do a ^= a when a and b point to the same address — it zeroes out.
  // That edge case is why temp swap remains the professional default. : if you do not know about the XOR logical operator, math boolean algebra, You may have to check out the boolean algebra table first.

  printf("Swapped using XOR : (a, b) = (%d, %d)\n", a, b);
  return 0;
}

