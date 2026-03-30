// The Problem : Create a struct BankAccount with account number, owner name, and balance. Write three functions — deposit, withdraw, and printBalance — each taking a pointer to a BankAccount. In withdraw, don't allow balance to go negative, print an error instead.

// Now that we need to create a struct BankAccount; We can also model money right? create another struct Money;
// I am going to do all the Money related calculations using Money; and the balance is a struct to money; lately I learnt the typedef. We are goining to enfore this here...

//==========================================INCLUDES HERE=============================================================================================

#include <stdio.h>
#include <math.h> 
#include <stdint.h> 
// no more unsigned int etc stuffs; 
// assuming there exists a megatrillionare, we are going to use uint64_t; Wait, is it first existing? Maybe, in the stdint library;
// tip : we need to use -lm to instruct the dynamic linker ld to link the glibm to glibc (linux systesm...); else we will get an error.



//======================================== DEFINITIONS HERE ===========================================================================================

// Now that I need to first put the definitions of the structs here....
typedef struct BankAccount bankaccount;
typedef struct Money money;
// Now that we have two types : bankaccount, another is money; money x = {...};, simple as that. But we are also allowed to use struct Money x = {...};


// Now that we need to put the definitions of the helper functions here....
// withdraw_money : a function that acts on a money type...
// we shall have it get an uint64_t, because none is going to spend a 0.2 dollars; we just spend in whole numbers.
void withdraw_money(money *data, uint64_t reduction);

// deposit_money : a function that deposits the given addition value...
void deposit_money(money *data, uint64_t addition);


// This function acts on the bankaccount, getting the balance from its money type...
// get_balance : a function returning a uint64_t.
uint64_t get_balance(bankaccount *acc);


//======================================= DECLARATIONS OF STRUCTS HERE =================================================================================

// 1) My Bank Account
typedef struct BankAccount {
    // you are not allowed to edit owner name;
    const char *owner; 
    // you are also not allowed to edit the account number...
    const int number;
    // but you can change the data of the money; the values, etc...
    money data;

} bankaccount ;

// 2) My Money 
typdef struct Money {
    // the number of withdrawals performed.
    int withdraws; 
    // the number of deposits performed.
    int deposits;
    // current value of the money inherited..(INHEREITED FROM FATHER....)
} money ;


//========================================= DECLARE THE HELPER FUNCTIONS =================================================================================
