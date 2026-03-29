// a struct is just a collection of data, but bound together as a single type..
#include <stdio.h> 

int main(void) {
    // define a struct which has three datatypes, an integer point, all the same.
    struct point {
        int x; 
        int y;
        int z;
    }; // note that unlike every other curly braces, we are required to put a ; here.

    // initialise a struct.
    struct point origin = {0,0,0};
    // or more explicitly.
    struct point ones = {.x=1, .y=1, .z=1};
    // or just initialise and assign later.
    struct point cuboid_dim;
    cuboid_dim.x = 10;
    cuboid_dim.y = 20;
    cuboid_dim.z = 30;

    printf("The origin is : (%d, %d, %d)\n", origin.x, origin.y, origin.z);
    printf("The ones point is : (%d, %d, %d)\n", ones.x, ones.y, ones.z);
    printf("The dimensions of cuboid is : (%d, %d, %d)\n", cuboid_dim.x, cuboid_dim.y, cuboid_dim.z);

    return 0;

}
