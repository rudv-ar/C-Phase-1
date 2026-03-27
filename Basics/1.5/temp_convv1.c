// Temperature again

// include the headers 
#include <stdio.h> 

// define the main function 
int main(void) {
  float cel = 0, far = 0;
  float lower = 0, step = 10, higher = 1000;

  printf("The Fahrenheit to Celsius Convertor \n");
  printf("\nFahrenheit(in degrees)\tCelsius(in degrees)\n");
  while (far <= higher) {
    cel = (float)5/9 * (far - 32);
    printf("\t%f\t\t%f\n", far, cel);
    far += step;
  }

}
