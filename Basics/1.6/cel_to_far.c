// Temperature again

// include the headers 
#include <stdio.h> 

// define the main function 
int main(void) {
  float cel = 0, far = 0;
  float lower = 0, step = 10, higher = 1000;

  printf("The Celsius to Fahrenheit Convertor \n");
  printf("\nCelsius(in degrees)\tFahrenheit(in degrees)\n");
  while (cel <= higher) {
    far = cel * (float)9/5 + 32;
    printf("\t%0.2f\t\t%0.2f\n", cel, far);
    cel += step;
  }

}
