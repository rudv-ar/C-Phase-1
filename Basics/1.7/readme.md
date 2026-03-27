# introducing For Loops in C....

From the beej's guide on C programming : A `for` loop has three parts inside a `()` whatever that is called, seperated by semicolons.

- Part 1 : Initialisation of stuffs. We do Initialisation - declare variables and set their initial value, we can even have multiple values, we can declare via operations, whaever and as much as initialisation seperated by commas.
- Part 2 : The condition - as long as this condition holds true, the for loop runs. The loop terminates as this condition turns false.
- Part 3 : The increment, or **do after each loop** statement. C does this after every loop.  After this comes the body of the for loop. Where we do what we like.

`for (initialisation; condition; do things) {... for's body ...}`


### Ex 1.5 : Modify the temperature program to print the table in the reverse order. That is from 300 degrees to 0. 

