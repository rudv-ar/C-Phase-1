// this is a rectangle struct.
// we are going to get 4 things. 
// top left point, bottom right point, and other points..

// before doing anything, we need to define a point data structure, which we shall use to define the rectangle.
// We need to define another function which we pass the points, and it calculates the distances...

// includes ---------------------------------------------------------------------------------------------------------------------
#include <stdio.h>
#include <math.h> // the newly discovered math lib. currently, we have string.h, stdio.h, stdint.h, stdbool.h, math.h; 5 libraries encountered.


#define EPSILON 0.0001f

// struct prototyping...........................................................................................................
// the point struct;
struct Point;

// the rectangle struct;
struct Rectangle;


// function prototyping..........................................................................................................
// now that I need to tell the compiler how my late comming functions look like, and for better visibility, I need to get instant
// access to how to write those functions, or call them, a centuries later when I am visiting the same code...
// We just do this... <return type> <function pointer if any *>functionname(arguments...); done, we have just defined the general
// form of a function prototype.

// 1) modify point : by default, we can use this function to initialise the point as well as modify this point later.
// Gets the pointer to the point, the data we want, so total 4 arguments.
// corrected : float arguments to match the struct Point fields which are float.
void edit_point(struct Point *point, float x, float y, float z);

// 2) distance_point : find the distance between two points; get pointers to two points. 
float distance_point(struct Point *A, struct Point *B);

// 3) calculate area : find the area provided that points of a rectangle ABCD are clockwise provided, A starting at topleft, topright bottomright, bottomleft.
float area_rect(struct Rectangle *rect);
// why am I used to take a pointer always? because they are not copied... just a read only practice for what is needed. No unwanted copy costs. I should pass real stuffs only if I need a copy of them.

// 4) Finally, we have the create rectangle function; which we can also use to edit the points in rectangle.
void edit_rect(struct Rectangle *rect, struct Point topleft, struct Point topright, struct Point bottomright, struct Point bottomleft);

// 5) helper function : comparing two floats is unreliable with == because floating point arithmetic introduces tiny rounding
// errors. Two values that should be mathematically equal will often differ by something like 0.00000001.
// We instead check if the difference between them is smaller than a tiny threshold called epsilon.
int float_equal(float a, float b);
// why? because float comparision is inaccurate; we need a small error correction value, we do it by probablity..



// struct definitions -----------------------------------------------------------------------------------------------------------

// 1) the Point data structure
struct Point {
    // it has a x point
    float x;
    // a y point
    float y;
    // a z point
    float z;
    // So you can just define a .x and .z and leave .y zero, etc easy stuff.
    // Warning, you cannot initialise a data inside struct...
};

// 2) the Rectangle data structure 
struct Rectangle {
    // we have it to get two structs : points.. But we are not going to have a pointer to points, because that point(or a copy of point) should belong to this struct. Enforcing mutability.
    struct Point topleft;
    struct Point bottomleft;
    struct Point topright;
    struct Point bottomright;
};


// define the functions ---------------------------------------------------------------------------------------------------------------

void edit_point(struct Point *point, float x, float y, float z) {
    // deref the point and edit the value, provided point exists initialised or even declared.
    (*point).x = x; // alternate way : point->x = x;
    (*point).y = y; // alternate way : point ...
    (*point).z = z; // same here..
}

// distance point.. 
float distance_point(struct Point *A, struct Point *B) {
    // now that we use basic maths stuff to calculate the distance. Where is my math module? UNLIKE; 
    // we do have a math.h in C; import it, the sqrtf(...) does the job.
    float x1 = A->x, y1 = A->y, z1 = A->z;
    float x2 = B->x, y2 = B->y, z2 = B->z;

    float dx = x2 - x1, dy = y2 - y1, dz = z2 - z1;
    float sx = dx*dx, sy = dy*dy, sz = dz*dz;
    return sqrtf(sx + sy + sz);
}

// helper : float equality check
int float_equal(float a, float b) {
    // fabs is the floating point absolute value from math.h
    // we check if the difference between two floats is smaller than epsilon instead of using ==
    return fabs(a - b) < EPSILON;
}

// area_rect
float area_rect(struct Rectangle *rect) {
    struct Point A = rect->topleft;
    struct Point B = rect->topright;
    struct Point C = rect->bottomright;
    struct Point D = rect->bottomleft;

    float width1  = distance_point(&A, &D);
    float width2  = distance_point(&B, &C);
    float length1 = distance_point(&A, &B);
    float length2 = distance_point(&C, &D);

    // now that we need to verify the properties of rectangle, it is a rectangle if width1 = width2, length1 = length2; if else it is not a rectangle;
    // if it is not a rectangle, we will be returning -1; just for a case to check; with this, if we need to check if the points are valid, we will return the area, else -1.0;
    // corrected : using float_equal instead of == to handle floating point rounding errors.
    if (float_equal(width1, width2) && float_equal(length1, length2)) {
        return width1 * length1;
    } else {
        return -1.0;
    }
}

void edit_rect(struct Rectangle *rect, struct Point topleft, struct Point topright, struct Point bottomright, struct Point bottomleft) {
    rect->topleft     = topleft;
    rect->topright    = topright;
    rect->bottomleft  = bottomleft;
    rect->bottomright = bottomright;
}

// the main function...

int main(void) {
    struct Point A, B, C, D;
    struct Rectangle rect1;

    // a unit square on the XY plane.
    // A at origin, B one unit right, C one unit right and one unit up, D one unit up.
    edit_point(&A, 0.0f, 0.0f, 0.0f);
    edit_point(&B, 1.0f, 0.0f, 0.0f);
    edit_point(&C, 1.0f, 1.0f, 0.0f);
    edit_point(&D, 0.0f, 1.0f, 0.0f);

    edit_rect(&rect1, A, B, C, D);

    float result = area_rect(&rect1);

    // sentinel check : area_rect returns -1.0 if the four points do not form a valid rectangle.
    if (result < 0) {
        printf("Not a valid rectangle.\n");
    } else {
        printf("Area : %.2f\n", result);
    }

    return 0;

    //alright.., finally, such a long struct!
}


