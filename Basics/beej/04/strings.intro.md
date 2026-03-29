# Strings in C — From First Principles

Strings in C can be expressed in two ways: **Arrays** and **Pointers**. When you assign a string to a pointer type, it is possible because strings are fundamentally arrays, and by the **array decay rule**, arrays decay into pointers. But there is a subtle and important difference between both.

---

## Part 1 — String Arrays

### By Me

String arrays are essentially arrays of `char`. These are defined on the **stack** if they are local variables, and in **`.data`** if they are global and initialized, or **`.bss`** if they are global and uninitialized.

We should explicitly fix the minimum size of the array to be `length of string + 1`. We account for that extra `1` for the null terminator `'\0'`.

```c
char name[6] = "hello";
```

`"hello"` exists in the `.rodata`, but when we assign it to `char name[6]`, 6 bytes of information is **copied** from `.rodata` into the stack (provided it is not a global variable), or into `.data` if it is a global initialized variable.

`"hello"` is essentially stored as `{'h', 'e', 'l', 'l', 'o', '\0'}`. When you just call `name`, it **decays** into a pointer pointing to the address of `'h'`. We can modify the characters in string arrays via `name[i] = 'A'`.

---

### Deep Explanation

#### Step 1 — What is memory?

Memory is just a long row of boxes. Each box holds **1 byte** and has a unique **address**:

```
address:  100  101  102  103  104  105  106 ...
value:    ???  ???  ???  ???  ???  ???  ??? ...
```

Every variable you declare occupies one or more of these boxes.

---

#### Step 2 — What is a `char`?

A `char` is exactly **1 byte**. Characters are stored as their **ASCII values**:

```
'h' = 104
'e' = 101
'l' = 108
'o' = 111
'\0' = 0
```

So storing `'h'` in memory looks like:

```
address:  100
value:    104   ← ASCII value of 'h'
```

---

#### Step 3 — What is a string literal `"hello"`?

The compiler sees `"hello"` and **at compile time** places it in `.rodata` (read-only data segment):

```
.rodata
address:  200  201  202  203  204  205
value:    'h'  'e'  'l'  'l'  'o'  '\0'
```

The `'\0'` is **automatically appended** by the compiler. You never write it manually.  
This happens **before your program even runs.**

The true type of `"hello"` is `const char[6]` — a constant array of 6 characters.

---

#### Step 4 — What is `char name[6]`?

You are asking the system to reserve **6 consecutive bytes** on the stack:

```
stack
address:  500  501  502  503  504  505
value:    ???  ???  ???  ???  ???  ???
```

Nothing is in them yet — just garbage values.

---

#### Step 5 — What happens during `char name[6] = "hello"`?

At runtime, **6 bytes are copied** from `.rodata` into the stack:

```
.rodata:  200  201  202  203  204  205
          'h'  'e'  'l'  'l'  'o'  '\0'
                    ↓ copied
stack:    500  501  502  503  504  505
          'h'  'e'  'l'  'l'  'o'  '\0'
```

Now `name` on the stack and `"hello"` in `.rodata` are **two completely independent copies.**  
Modifying one does NOT affect the other.

---

#### Step 6 — What is `name` alone?

`name` is the array itself. But when you use it in an expression, it **decays** into a pointer to its first element:

```
name  →  address 500   (points to 'h')
```

This is why `printf("%s", name)` works — `printf` receives address `500` and **walks forward byte by byte** until it hits `'\0'`:

```
500 → 'h'
501 → 'e'
502 → 'l'
503 → 'l'
504 → 'o'
505 → '\0'  ← STOP
```

---

#### Step 7 — Why can you modify `name` but not `"hello"`?

```c
name[0] = 'H';  // writing to address 500 on stack → OK ✓
```

Stack memory belongs to you. You can read and write freely.

`.rodata` is **marked read-only by the OS** with permissions `r--` (read, no write, no execute).  
Any attempt to write there triggers a **segfault (SIGSEGV)**:

```c
char *s = "hello";
s[0] = 'H';  // writing to address 200 in .rodata → CRASH ✗
```

---

#### Step 8 — Global strings and memory segments

```c
char name[6] = "hello";  // global, initialized   → .data
char name[6];            // global, uninitialized  → .bss
```

| Segment | What lives there | Mutable | Stored in executable |
|---------|-----------------|---------|----------------------|
| `.rodata` | string literals | No | Yes |
| `.data` | global initialized variables | Yes | Yes |
| `.bss` | global uninitialized variables | Yes | No (only size recorded) |
| stack | local variables | Yes | No |

**Why does `.bss` exist?**  
If you declare `char name[1000];` globally, it would waste 1000 bytes of disk space in the executable. Instead, the OS just reads "`.bss` needs 1000 bytes", allocates it in RAM, and **zero-fills** it at startup. Saves disk space.

---

#### Step 9 — Let the compiler count the size

```c
char name[] = "hello";  // compiler automatically sets size to 6
```

No need to manually count. The compiler figures it out. Safer and cleaner.

---

#### Full Mental Model — String Arrays

```
compile time:  "hello" placed in .rodata at address 200  (const char[6])
runtime:       stack allocates 6 bytes at address 500
runtime:       6 bytes copied from address 200 → 500
runtime:       name decays to address 500 when used in expressions
```

---

## Part 2 — Char Pointers

### By Me

Char pointers are just **pointers** — they are exactly what they are named as. They are pointers to characters that reside in `.rodata`.

Every **string literal** is stored in `.rodata` as an array of characters, as we saw in Part 1. But when we assign one to a `char` pointer:

```c
const char *s = "hello";
```

We are essentially storing the **address of the first character** of the array in `.rodata`. This is possible because arrays **decay into a pointer** pointing to the first element.

We **cannot modify** the data because it resides in `.rodata` with permissions `r--`. Any attempt to write there will result in a `SIGSEGV`.

`"hello"` is stored as `{'h', 'e', 'l', 'l', 'o', '\0'}` in `.rodata`. When we write `const char *s = "hello"`, `s` simply holds the address of `'h'` in `.rodata`. It does not own or copy the data.

---

### Deep Explanation

#### Step 1 — What is a pointer?

A pointer is a variable that stores an **address**. On a 64-bit system, a pointer is 8 bytes (enough to hold any memory address).

```
address:  900
value:    200    ← s stores address 200 (where 'h' lives in .rodata)
```

So `s` itself lives at address `900` on the stack, and its value is `200`.

---

#### Step 2 — What happens during `const char *s = "hello"`?

```
compile time:   "hello" placed in .rodata at address 200
                200  201  202  203  204  205
                'h'  'e'  'l'  'l'  'o'  '\0'

runtime:        s allocated on stack at address 900
                s = 200   ← just stores the address, NO copy made
```

Full picture:

```
stack:          .rodata:
[ s = 200 ] ──→ ['h']['e']['l']['l']['o']['\0']
  addr 900        200  201  202  203  204  205
```

No data is copied. `s` is just a sticky note with address `200` written on it.

---

#### Step 3 — Why `const char *` and not `char *`?

```c
char *s       = "hello";  // compiles but dangerous — drops const
const char *s = "hello";  // correct — respects read-only nature of .rodata
```

`"hello"` is `const char[6]`. Assigning it to `char *` drops the `const`, meaning the compiler won't warn you if you try to write to it. At runtime it still crashes because `.rodata` is `r--`.

`const char *s` tells the compiler: *"I promise not to write through this pointer."* If you try, it catches it **at compile time** — before it becomes a runtime crash.

---

#### Step 4 — What does `s == k` mean for two `char *`?

```c
const char *s = "hello";
const char *k = "hello";

s == k   // comparing address 200 == address 300 ?
         // NOT comparing 'h','e','l','l','o'
```

`==` on pointers asks: **"Do these two pointers point to the same address?"**  
Not: **"Do the characters they point to spell the same word?"**

This is why `==` gives wrong results for string comparison. Use `strcmp` instead:

```c
strcmp(s, k) == 0   // walks both strings character by character → correct ✓
```

---

#### Step 5 — What does `*s == *k` mean?

`*s` dereferences the pointer — it gives you the value **at** the address `s` holds:

```c
*s   →   value at address 200   →   'h'
*k   →   value at address 300   →   'h'

*s == *k   →   'h' == 'h'   →   true
```

But this only compares the **first character**. It would give wrong answers for `"hello"` vs `"hi"` — both start with `'h'`.

This is exactly what `strcmp` does internally, but across **every character**:

```c
// rough internal logic of strcmp
while (*s == *k) {
    if (*s == '\0') return 0;  // both ended → equal
    s++;
    k++;
}
return *s - *k;  // difference at first mismatch
```

---

#### Step 6 — Array vs Pointer side by side

```c
char arr[] = "hello";   // ARRAY  — copies 6 bytes onto stack, owns the memory
const char *s = "hello"; // POINTER — stores address of .rodata, owns nothing
```

```
Array:
stack: ['h']['e']['l']['l']['o']['\0']   ← your copy, mutable
         500  501  502  503  504  505

Pointer:
stack: [ 200 ]  ──→  .rodata: ['h']['e']['l']['l']['o']['\0']
         900                    200  201  202  203  204  205
```

| Property | `char arr[]` | `const char *s` |
|----------|-------------|-----------------|
| Memory location | stack | pointer on stack, data in `.rodata` |
| Owns data | Yes | No |
| Mutable | Yes | No (`r--`) |
| `sizeof` | 6 (full array) | 8 (pointer size) |
| Data copied | Yes | No |

---

#### Full Mental Model — Char Pointers

```
compile time:   "hello" placed in .rodata at address 200
runtime:        s allocated on stack at address 900
runtime:        s = 200  (address stored, NO copy)
runtime:        *s = 'h' (first character at address 200)
runtime:        s++ moves to address 201, now *s = 'e'
```

The pointer is just a **navigator**. It knows where the string starts. It walks forward one byte at a time. It stops at `'\0'`. That's all there is to it.

---

## Summary — The Complete Picture

```
"hello"              →   const char[6] in .rodata   (permanent, read-only)
char arr[] = "hello" →   copy on stack              (temporary, mutable)
const char *s ="hello"→  pointer to .rodata          (no copy, read-only)
```

- `'\0'` is always there — it is what makes a sequence of bytes a **string**
- `==` on pointers compares **addresses**, not characters — use `strcmp`
- `*s` gives you the **first character only**, not the whole string
- `.rodata` is `r--` — write to it and the OS sends `SIGSEGV`
- Arrays **decay** into pointers when used in expressions — that's why both work with `printf`, `strcmp`, etc.

