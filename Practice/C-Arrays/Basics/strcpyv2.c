// let's try writing a memory safe string copy. 
// - Takes a destination buffer, it's size, and a source string (probably address of the string)
// - Never writes past the buffer boundary
// - Always null terminates the destination 
// - Returns the number of byptes actualy written
// - BUT without using strcpy or strncpy 

//----------IMPORTS------------------------------------------------------

#include <stdio.h> 
// include the standard input output library

//----------MACROS-------------------------------------------------------

#define STR_BUFFER_SIZE 1024 
// default buffer size for string : 1024 bytes = 1kb 

//---------FUNCTION DEFS--------------------------------------------------

// define a function to find the length of a string : array of chars; it takes a pointer to the arr, compares the bytes at each address untill it is '\0', returns an integer.

int strlength(char *arr);

// define a function to safely copy the data from one string to another, taking the size of from and to. along with pointers to the strings.
int strcopy(char *from, char *to, int buffer);



//--------FUNCTIONS---------------------------------------------------------

// function to find the length of string - including the null-terminator
int strlength(char *arr) { // get a pointer to array of chars
    int len = 0; // set the initial length to zero
    for (int i = 0; *(arr + i) != '\0'; i++) { // initialise a index 0 variable i, increment it by 1 untill the arr[i] = *(arr + i) is not equal to null terminator. ie, the character at the index should not be equal to the null terminator. 
        len++; // count the characters in it.
    }
    // return len + 1; // include the +1 for accounting the null terminator also for some safety purposes that I would like to make in future. But I don't need it.. Note that only the array strings are modifiable and not the char pointers to string at .data. 
    return len;
}


// the strcopy function takes the two pointers; and copies from address to the two address the char values one by one until from reaches the null pointer. | alternatively, we can use i <= strlength(from); 
int strcopy(char *from, char *to, int buffer) {
    if (strlength(from) + 1 <= buffer) { // if the size of the to buffer is sufficient enought to accomodate the characters from the 'from' arry, ie, if the size of from size is less than or equal to the tosize, then we proceed with the copy opration.
        int i = 0;
        for ( ; *(from + i) != '\0'; i++) { // loop until the from string/array reaches the null terminator, and gradually increment the index to next address of the pointers, get the value and overwrite the value in to string to the value form the from_string.
            *(to + i) = *(from + i);
        }

        *(to + i) = '\0'; // add the null terminator
    }
    return buffer - strlength(from) - 1; // finally return the remaining size; in bytes. (integer type); if this were a negative value, we can understand that the copy operation did not happen and the to string is same as previous. If the return value is greater than zero, ie positive, it means that tere is still extra space for the to strig to write to. it returns the extra space. If it is zero, then it means it is a exact copy.

}


//--------MAIN--------------------------------------------------------------

// define the main function 
int main(void) { 
    // initialise a string pointer which points to the .data "hello"; which by default has null terminator.
    char arr[10] = "hello";
    // what is the size of this arr? 
    int size = sizeof(arr);  
    char arr1[5] = "a"; // initialise with some thing like this...
    char arr2[8] = "b";
    printf("%s : %d : %d\n", arr, strlength(&arr[0]), size);
    printf("%s : %d : %ld\n", arr1, strlength(&arr1[0]), sizeof(arr1));
    
    int copy1 = strcopy(arr, arr1, sizeof(arr1)); // note that we need to pass the strlength of array + 1 for this which includes the 
    int copy2 = strcopy(arr, arr2, sizeof(arr2));
    // here strlength returns 1, wheras the sizeof returns 5; you know that is the difference. So, for strcopy, it is always prefered to use the sizeof instead of strlength - because it counts the string length and not the array length.

    printf("%s length : %d : %ld - %s\n", arr1, strlength(arr1), sizeof(arr1), copy1 >= 0 ? "Copied Successfully!" : "Copy Failed");
    printf("%s length : %d : %ld - %s\n", arr2, strlength(arr2), sizeof(arr2), copy2 >= 0 ? "Copied Successfully!" : "Copy Failed");

    return 0;
}
