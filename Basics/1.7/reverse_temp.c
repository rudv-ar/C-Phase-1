
# include<stdio.h> // weird spacings are also allowed. but not inside this : <...> 

int main(void) {
  printf("Cel\tFar\n");
  for (float cel = 300, far = 0; cel >= 0; cel-=10) {
    printf("%.2f\t%.2f\n", cel, ((float)9/5 * cel) + 32 );
  }
}


