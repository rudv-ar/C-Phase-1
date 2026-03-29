// here I am going to create a student struct.
// the student has a name which is char * type, not modifyable, a string type... Why should a student change his name?
// the student roll number, for sake we have it as an integer, but represent it with more robust hexdecimal form.
// the marks obtained by the student, which is of type float; we do not need a double precision for the marks of student.
// the final is char : the grade of the student. ABCDE, enough.

#include <stdio.h>


struct student {
    char *name;
    int roll;
    float marks;
    char grade;
};





//=================================FUNCTION DECLARATIONS=================================================================================
// now define the functions
void enroll_student(struct student *data, char *name, int roll, float marks, char grade) {
    // for getting the struct, we need to access that data. but wait, the type of the data is struct student * and not struct student.
    // we got a pointer to the struct, not the struct itself. so how do we access the elements in struct? 
    // simple as that: just dereference the struct...
    // struct student someone = *(data). but alas!. Structs will make a copy, you made a copy of data struct to a someone struct. there are not changes made in the original data struct passed. so what next?
    // stand by and assign...

    (*data).name = name; // this should not be done this way? we are making the char pointer to point at the address of char *name supplied, essentially lives in .rodata and not in stack mem.
    (*data).roll = roll;
    (*data).marks = marks;
    (*data).grade = grade; // remember that : TYPE OF LHS == TYPE OF RHS; else potential type mismatch, compile errors, or worst case UB SIGSEGV.
    // alternatively : we can use this nice looking symbol to dereference the pointer directly and modify the data in struct : data->grade = ...
    // but I need it to be more explicitly done like this, cause I always need to remember, not by accident that it is a pointer to struct... not the actual struct....
}

// define the print_studata function.
void print_studata(struct student *data) {
    printf("[+] Printing student data...\n");
    printf("[Name] : %s\n", (*data).name);
    printf("[Roll Number] : %#06x \n", (*data).roll);
    printf("[marks] : %.3f\n", (*data).marks);
    printf("[Grade] : %c\n", (*data).grade);
    
   
}

//=================================FUNCTION MAIN========================================================================================
// define a main function 
int main(void){
    // define a student struct; 
    // now that we are going to initialise the student named orio, then fill his credentials.
    struct student orio = {}; // empty struct.

    enroll_student(&orio, "orio", 2323, 99.979, 'A');
    // now that we shall try printing student data of orio...
    print_studata(&orio);


    return 0;
}



