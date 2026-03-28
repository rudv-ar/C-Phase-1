# Pointers in C — A Complete Guide

---

## Table of Contents

1. [What is Memory?](#1-what-is-memory)
2. [Variables and Addresses](#2-variables-and-addresses)
3. [What is a Pointer?](#3-what-is-a-pointer)
4. [Declaring and Initializing Pointers](#4-declaring-and-initializing-pointers)
5. [The Two Key Operators: `&` and `*`](#5-the-two-key-operators--and-)
6. [Pointer Types Matter](#6-pointer-types-matter)
7. [Pointer to a Pointer (`**`)](#7-pointer-to-a-pointer-)
8. [`void *` — The Generic Pointer](#8-void----the-generic-pointer)
9. [Pointers and Arrays](#9-pointers-and-arrays)
10. [Pointer Arithmetic](#10-pointer-arithmetic)
11. [Pointers and Strings](#11-pointers-and-strings)
12. [Pointers and Functions](#12-pointers-and-functions)
13. [Common Mistakes](#13-common-mistakes)
14. [Full Memory Layout Summary](#14-full-memory-layout-summary)

---

## 1. What is Memory?

Your computer's RAM can be thought of as a **giant array of bytes**, where every byte has a unique **address** — a number, usually shown in hexadecimal. This is based on the c23 standard; Back in c11, we do not cast the pointers using (void *); This is because while we format the pointer, it returns an address, using "%p" and %p expects a void. Let's check that?

```
Address:   0x1000  0x1001  0x1002  0x1003  0x1004  ...
           ┌──────┬──────┬──────┬──────┬──────┐
Value:     │  42  │  00  │  FF  │  48  │  65  │  ...
           └──────┴──────┴──────┴──────┴──────┘
```

When you declare a variable, the compiler:
1. Reserves some bytes in memory for it
2. Associates a name with that address
3. Tracks its type so it knows how many bytes to read/write

---

## 2. Variables and Addresses

```c
int k = 100;
```

This does three things:
- Allocates `4 bytes` on the stack (on most 64-bit systems, `int` = 4 bytes)
- Stores the value `100` at that location
- Names that location `k`

```
Stack memory:
┌──────────────────────────────┐
│  k  │  value: 100            │
│     │  address: 0x7ffd1234   │
│     │  size: 4 bytes         │
└──────────────────────────────┘
```

You can get that address with the **address-of operator** `&`:

```c
printf("%p\n", (void *)&k);  // prints something like 0x7ffd1234
```

---

## 3. What is a Pointer?

A **pointer** is a variable that **stores a memory address**.

That's it. It's just a number — but that number is the address of something else in memory.

```c
int k = 100;
int *p = &k;   // p holds the address of k
```

```
Stack
┌───────────────┐         ┌───────────────┐
│  p            │ ──────► │  k            │
│  value:       │         │  value: 100   │
│  0x7ffd1234   │         │  @ 0x7ffd1234 │
│  @ 0x7ffd1240 │         └───────────────┘
└───────────────┘
```

> **Key insight:** `p` is itself a variable. It lives somewhere in memory, has its own address, and holds a value — which happens to be another address.

---

## 4. Declaring and Initializing Pointers

```c
int   *p;    // pointer to int
char  *c;    // pointer to char
float *f;    // pointer to float
void  *v;    // pointer to "anything" (generic)
```

The `*` in the declaration means **"this variable is a pointer to..."**.

> ⚠️ An uninitialized pointer is **dangerous**. It points to a random memory location.

```c
int *p;      // ⚠️  p contains garbage — undefined behavior to use
int *p = NULL;  // ✅ safe — explicitly points to nothing
```

Always initialize pointers before using them.

---

## 5. The Two Key Operators: `&` and `*`

These are the two most fundamental pointer operators.

### `&` — Address-of Operator

Takes a variable and returns **its address**.

```c
int k = 100;
int *p = &k;   // &k = "give me the address of k"
```

### `*` — Dereference Operator

Takes a pointer and returns **the value at that address**.

```c
int val = *p;   // *p = "give me the value p points to"
// val == 100
```

You can also write through a pointer:

```c
*p = 200;       // changes k to 200
printf("%d\n", k);  // prints 200
```

### Summary Table

| Expression | Type      | Meaning                        | Value (example)  |
|------------|-----------|--------------------------------|------------------|
| `k`        | `int`     | value of k                     | `100`            |
| `&k`       | `int *`   | address of k                   | `0x7ffd1234`     |
| `p`        | `int *`   | value of p (= address of k)    | `0x7ffd1234`     |
| `*p`       | `int`     | value at the address p holds   | `100`            |
| `&p`       | `int **`  | address of p itself            | `0x7ffd1240`     |

---

## 6. Pointer Types Matter

The type of a pointer tells the compiler **how many bytes to read** when dereferencing.

```c
int    x = 1000;
int    *pi = &x;   // reads 4 bytes
char   *pc = (char *)&x;  // reads 1 byte
double *pd = (double *)&x; // reads 8 bytes — DANGEROUS (reading out of bounds)
```

```
Memory at &x:
┌──────┬──────┬──────┬──────┐
│  E8  │  03  │  00  │  00  │   ← 4 bytes = int 1000 (little-endian)
└──────┴──────┴──────┴──────┘
  ▲         ▲
  *pc       *pi reads all 4
reads 1
```

The **type** attached to a pointer is not just metadata — it controls every operation: dereferencing, arithmetic, and assignment.

---

## 7. Pointer to a Pointer (`**`)

Since a pointer is just a variable, you can take the address of a pointer too. That gives you a **pointer to a pointer**.

```c
int   k = 100;
int  *p  = &k;    // p  points to k
int **pp = &p;    // pp points to p
```

```
Stack
┌──────────┐      ┌──────────┐      ┌──────────┐
│  pp      │ ───► │  p       │ ───► │  k       │
│ int **   │      │ int *    │      │ int      │
│ @ 0x...C │      │ @ 0x...8 │      │ @ 0x...4 │
│ val=...8 │      │ val=...4 │      │ val=100  │
└──────────┘      └──────────┘      └──────────┘
```

### Dereferencing levels

```c
**pp == 100   // dereference twice → value of k
 *pp == p     // dereference once  → value of p (address of k)
  pp == &p    // the pointer itself → address of p
```

### Getting each address with `printf`

```c
printf("%p\n", (void *)&k);   // address of k     → needs (void *)
printf("%p\n", (void *)p);    // address of k     → p is int *,  cast to void *
printf("%p\n", (void *)&p);   // address of p     → &p is int **, cast to void *
printf("%p\n", (void *)pp);   // address of p     → pp is int **, cast to void *
printf("%p\n", (void *)&pp);  // address of pp    → &pp is int ***, cast to void *
```

> Every pointer, regardless of type (`int *`, `int **`, `char *`, etc.), must be cast to `(void *)` when passed to `printf("%p")`. This is why `(void *)` matters.

---

## 8. `void *` — The Generic Pointer

`void *` is a pointer with **no associated type**. It holds an address but makes no claim about what lives there.

### Why it exists

C needs a way to write **generic** functions that work on any type of data — like `malloc`, `memcpy`, `qsort`, and `printf("%p")`.

```c
void *malloc(size_t size);  // returns a pointer to "something"
```

### Implicit conversion

In C, any pointer can be **implicitly converted** to/from `void *`:

```c
int   *pi = malloc(sizeof(int));   // void * → int *  (no cast needed in C)
void  *v  = pi;                    // int *  → void * (no cast needed in C)
```

> In C++, this implicit conversion does **not** exist. You must cast explicitly.

### What you cannot do

Because the type is unknown, you cannot dereference or do arithmetic on a raw `void *`:

```c
void *v = &k;

*v;     // ❌ error: cannot dereference void pointer
v++;    // ❌ error (in standard C): sizeof(void) is undefined

*(int *)v;   // ✅ cast first, then dereference
```

### Why `printf("%p")` needs `(void *)`

`%p` is defined by the C standard to take exactly a `void *`. Passing `int *`, `char *`, or `int **` without casting is **technically undefined behavior**, even though in practice all pointer types are the same size on modern systems.

```c
int k = 100;
int *p = &k;

printf("%p\n", (void *)p);    // ✅ int *   → void *
printf("%p\n", (void *)&p);   // ✅ int **  → void *
printf("%p\n", (void *)&k);   // ✅ int *   → void *
```

---

## 9. Pointers and Arrays

Arrays and pointers are deeply related in C.

```c
int arr[3] = {10, 20, 30};
int *p = arr;   // arr decays to a pointer to its first element
```

```
Memory:
┌──────┬──────┬──────┐
│  10  │  20  │  30  │
└──────┴──────┴──────┘
  ▲
  p = &arr[0]
```

```c
*p       == arr[0] == 10
*(p + 1) == arr[1] == 20
*(p + 2) == arr[2] == 30
```

`arr[i]` is literally defined as `*(arr + i)` in C. They are equivalent.

> ⚠️ **Array name ≠ pointer**: `arr` is not a variable — you cannot do `arr++`. `p` is a variable — you can do `p++`.

---

## 10. Pointer Arithmetic

When you add/subtract an integer from a pointer, C scales the operation by the **size of the pointed-to type**.

```c
int *p = arr;   // assume arr is at 0x1000, int = 4 bytes

p + 0  →  0x1000   // arr[0]
p + 1  →  0x1004   // arr[1]  (moved 4 bytes)
p + 2  →  0x1008   // arr[2]  (moved 8 bytes)
```

This is why the type of a pointer matters so much — arithmetic only makes sense if the compiler knows the element size.

```c
char *c = (char *)arr;

c + 0  →  0x1000   // byte 0
c + 1  →  0x1001   // byte 1  (moved 1 byte)
c + 4  →  0x1004   // = p + 1
```

---

## 11. Pointers and Strings

In C, a string is just an array of `char` ending in a null terminator `'\0'`.

```c
char *hello = "Hello World!";
```

`"Hello World!"` is a **string literal** — it lives in the **read-only data segment** (`.rodata`) of the binary, placed there at compile time.

```
.rodata (read-only, static lifetime)
┌───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬────┐
│ H │ e │ l │ l │ o │   │ W │ o │ r │ l │ d │ ! │ \0 │
└───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴────┘
  ▲
  hello (a char * on the stack/data segment pointing here)
```

| Expression   | Type     | Value           |
|--------------|----------|-----------------|
| `hello`      | `char *` | address of `'H'` |
| `*hello`     | `char`   | `'H'`           |
| `hello[0]`   | `char`   | `'H'` (same as `*hello`) |
| `&hello`     | `char **`| address of the pointer variable |
| `(void *)&hello` | `void *` | address of pointer, safe for `%p` |

> ⚠️ Never modify a string literal. `hello[0] = 'h'` is **undefined behavior** — the memory is read-only.

---

## 12. Pointers and Functions

### Pass by value vs pass by pointer

C is **pass-by-value**. A function gets a *copy* of arguments. To modify the original, pass a pointer.

```c
// ❌ does NOT modify x
void increment(int n) {
    n++;  // modifies the copy, not the original
}

// ✅ modifies x through the pointer
void increment(int *n) {
    (*n)++;  // dereference and increment
}

int x = 5;
increment(&x);   // pass address of x
// x is now 6
```

### Returning pointers

Be careful: never return a pointer to a local variable — it goes out of scope.

```c
int *bad() {
    int x = 10;
    return &x;   // ⚠️ DANGER: x is destroyed when function returns
}

int *good() {
    int *x = malloc(sizeof(int));
    *x = 10;
    return x;    // ✅ heap-allocated, survives function return
}
```

---

## 13. Common Mistakes

### 1. Uninitialized pointer (wild pointer)

```c
int *p;
*p = 5;   // ❌ p points to random memory — undefined behavior / crash
```

### 2. Null pointer dereference

```c
int *p = NULL;
*p = 5;   // ❌ crash — cannot dereference NULL
```

### 3. Dangling pointer

```c
int *p = malloc(sizeof(int));
free(p);
*p = 5;   // ❌ use-after-free — undefined behavior
p = NULL; // ✅ good habit: nullify after freeing
```

### 4. Off-by-one in arrays

```c
int arr[3];
int *p = arr + 3;
*p = 99;   // ❌ out of bounds — arr[3] doesn't exist
```

### 5. Missing `(void *)` with `%p`

```c
printf("%p\n", &k);          // ⚠️ technically UB
printf("%p\n", (void *)&k);  // ✅ correct
```

---

## 14. Full Memory Layout Summary

```
High addresses
┌──────────────────────────────────────┐
│           Stack                      │
│  local variables, function frames   │
│  grows downward ↓                   │
│                                      │
│  int k     @ 0x7ffd1000  value=100  │
│  int *p    @ 0x7ffd1008  value=0x7ffd1000  │
│  int **pp  @ 0x7ffd1010  value=0x7ffd1008  │
├──────────────────────────────────────┤
│           Heap                       │
│  malloc/free allocations             │
│  grows upward ↑                     │
├──────────────────────────────────────┤
│           BSS Segment                │
│  uninitialized global/static vars    │
├──────────────────────────────────────┤
│           Data Segment               │
│  initialized global/static vars      │
├──────────────────────────────────────┤
│           .rodata (read-only)        │
│  string literals, const data         │
│  "Hello World!\0"  @ 0x402004        │
├──────────────────────────────────────┤
│           Text Segment               │
│  compiled machine code               │
└──────────────────────────────────────┘
Low addresses
```

### The chain of indirection

```
pp (int **)        p (int *)          k (int)
┌──────────┐       ┌──────────┐       ┌──────────┐
│ 0x...008 │ ────► │ 0x...000 │ ────► │   100    │
└──────────┘       └──────────┘       └──────────┘
@ 0x...010        @ 0x...008         @ 0x...000

&pp → 0x...010    (int ***)
 pp → 0x...008    (int **)   — address of p
*pp → 0x...000    (int *)    — value of p = address of k
**pp → 100        (int)      — value of k
```

---

> **Golden rule:** Every `*` in a type declaration adds one level of indirection. Every `*` in an expression removes one level (dereferences). Every `&` in an expression adds one level (takes address).

