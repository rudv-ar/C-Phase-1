// ======================================================== INCLUDES ======================================================

#include <stdio.h> 

// ======================================================== DEFINITIONS ===================================================

void insertionSort(int *arr, int size);
void printArray(int *arr, int size);

// ======================================================== MAIN FUNCTION =================================================

int main(void) {
    int array[] = {1, 5, 7, 21, 34, 11, 10, 90, -21, 44, 33}; 
    int size = sizeof(array) / sizeof(array[0]); 

    printf("Array Before Sorting : ");
    printArray(&array[0], size); 
    insertionSort(&array[0], size);
    printf("Array After Sorting : ");
    printArray(&array[0], size);

}

// ======================================================== DECLARE THE FUNCTIONS =========================================

void insertionSort(int *arr, int size){
    for (int i = 1; i < size; i ++) {
        int key = *(arr + i);
        int l = i - 1;
        for (;l >= 0 && *(arr + l) > key; l--) {
            *(arr + l + 1) = *(arr + l); 
        }
        *(arr + l + 1) = key;
    }
}


void printArray(int *arr, int size) {
    for (int i = 0; i < size; i++) {
        printf("%d ", *(arr + i));
    }
    puts(""); // put has it's own addtion of \n, no \n required additionally.
}


// ######################################################### EOF ! CAREFULL ###########################################################################
// Ah, by the end of this code, I have understood the importance of comments.. Uncommented code == longer time to get what exactly is happening; thou you might me an expert, but comments like this save atleast 5 seconds; per function block; at maximum, hours...

