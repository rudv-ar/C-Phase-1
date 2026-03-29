// this is a rectangle struct.
// we are going to get 4 things. 
// top left point, bottom right point, and other points..

// before doing anything, we need to define a point data structure, which we shall use to define the rectangle.
// We need to define another function which we pass the points, and it calculates the distances...

// includes ---------------------------------------------------------------------------------------------------------------------
#include <stdio.h>


// function prototyping..........................................................................................................
// now that I need to tell the compiler how my late comming functions look like, and for better visibility, I need to get instant
// access to how to write those functions, or call them, a centuries later when I am visiting the same code...
// We just do this... <return type> <function pointer if any *>functionname(arguments...); done, we have just defined the general
// form of a function prototype.

// 1) modify point : by default, we can use this function to initialise the point as well as modify this point later.
// Gets the pointer to the point, the data we want, so total 4 arguments.
void edit_point(struct Point *point, float x, float y, float z);
// 

// struct definitions -----------------------------------------------------------------------------------------------------------

// 1) the Point data structure
struct Point {
    // it has a x point
    float x = 0;
    // a y point
    float y = 0;
    // a z point
    float z = 0;
    // each initialised to zero by default. So you can just define a .x and .z and leave .y zero, etc easy stuff.
};

// 2) the Rectangle data structure 
struct Rectangle {
    // we have it to get two structs : points.. But we are not going to have a pointer to points, because that point(or a copy of point) should belong to this struct. Enforcing mutability.
    struct Point topleft;
    struct Point bottomleft;
    struct Point topright;
    struct Point bottomright;
};



