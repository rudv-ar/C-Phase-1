// this time, the same code, but written using pointer dynamics instead of accessing array members; ie, everywhere, you see *(arr + i); 
// ======================================================== INCLUDES ======================================================

#include <stdio.h> 

// ======================================================== DEFINITIONS ===================================================

// 1) First definition.
void insertionSort(int *arr, int size);
// 2) Second definition.
void printArray(int *arr, int size);
// The function definiiton explain themselvs.
// I guess using this style of function naming is good and seems visually appealing to me than sname_case.


// ======================================================== MAIN FUNCTION =================================================

int main(void) {
    int array[] = {1, 5, 7, 21, 34, 11, 10, 90, -21, 44, 33}; // array[] means that we are telling the compiler : CALCULATE THE SIZE OF THIS ARRAY YOURSELF.
                                                      // This is a good choice if I do not want to risk my life, counting wrong - array[10] instead of array[11]; a sigsegv, buffer overflow, undefined behaviour. Alas!
    int size = sizeof(array) / sizeof(array[0]); 

    printf("Array Before Sorting : ");
    printArray(&array[0], size); // I would like this mentioned explicitly : &array[0]; but still passing just array is no wrong as it decays into pointer.
    insertionSort(&array[0], size);
    printf("Array After Sorting : ");
    printArray(&array[0], size);

}

// ======================================================== DECLARE THE FUNCTIONS =========================================

// take a pointer to the array, and the size of the array; *arr, arr[], all denote the same. pointer to the first element of the array;
void insertionSort(int *arr, int size){
    // create a for loop, where we loop through each element of the array.
    // The first index should always be 1, as we would need to compare it with 0th index for sorting purposes.
    // loop as long as the size of the array is greater than the index (to avoid buffer overflow), gradually incrementing the index each loop
    for (int i = 1; i < size; i ++) {
        // fix the key; now that we need the key which is arr[i], or alternatively, *(arr + i) => the array pointer equivalence; 
        int key = *(arr + i);
        // now that we need a left value, the value initialised the index immeditaely left to the current index.
        int l = i - 1;
        // we keep on comparing the left values as long as the l is valid, ie greater than 0 and the element at l is greater than current key;
        while (l >= 0 && *(arr + l) > key) {
            // if this is true, then, assign the immediate right of the l_index the current l value.
            *(arr + l + 1) = *(arr + l); // --> the *(arr + i) will be overwritten in the first loop; but don't worry, we have a copy of it in the key.
            // decrement the l in order the compare the key with the next l_index, and this repeats as long as the condition satisfies.
            l--;
        }
        // now that the while ends when the given condition is false, we have a hole here and have not assigned our key value as it is previously overwritten has to be reassigned in this empty space.
        *(arr + l + 1) = key;
    }
    // as this for loop ends, we would have finally sorted the array.
    // But in order to return an array, we would possibly need to use malloc as we do not know the size of the array untill runtime.
}


// loop through and print the array;
void printArray(int *arr, int size) {
    for (int i = 0; i < size; i++) {
        printf("%d ", *(arr + i));
    }
    puts("\n"); // using my fav puts...
}


// ######################################################### EOF ! CAREFULL ###########################################################################
// there is also a for loop version for this code with no while, let's try that in v3? In fact, the for loop is just a generalisation of while loop.
// for (;;) {...}; this is a while loop with no conditions, exactly equal to while (true) {...}
