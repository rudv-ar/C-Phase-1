# C Pointers — Complete Deep Explanation
# From first principles. Every concept, every bug, every memory layout.
# Generated from first-day learning session.
# Rename this file to .md

---

## 0. Before Pointers — What Every Variable Actually Is

Every variable in C has two things. Not one. Two.

```c
int a = 10;
```

```
        a
   ┌─────────┐
   │   10    │   ← VALUE  : what you stored
   └─────────┘
   0x00000021    ← ADDRESS : where it lives in memory
```

RAM is a giant strip of numbered boxes. Each box = 1 byte. Each box has an address.
When you declare `int a = 10`, the compiler reserves 4 boxes (int = 4 bytes) somewhere
in memory, gives that location a name `a`, and puts `10` in it.

**Where does it live?**

```
if declared inside a function  → STACK   (local, temporary, auto-managed)
if declared globally, initialized → .data  (lives for entire program lifetime)
if declared globally, uninitialized → .bss (zeroed by OS at startup)
```

The address is **virtual** — not physical RAM directly. The OS + MMU (Memory Management Unit)
maps virtual addresses to physical RAM transparently. Your program never sees real RAM addresses.

---

## 1. What is a Pointer?

A pointer is a variable that stores an **address** instead of a normal value.

```c
int a = 10;
int *p = &a;
```

`&a` = "give me the address of a" = `0x00000021`

Now `p` holds that address:

```
        a                        p
   ┌─────────┐              ┌─────────────────────┐
   │   10    │              │     0x00000021       │
   └─────────┘              └─────────────────────┘
   0x00000021                     0x00000033
```

`p` is just a variable. It has its own address (`0x00000033`).
Its value is `0x00000021` — the address of `a`.

**On x86-64 (your Arch Linux machine):**
- All pointers are **8 bytes** regardless of what they point to
- An address is 64 bits = 8 bytes = 16 hex digits
- Example: `0x00007ffd8a3b1c20`

---

## 2. The * Symbol — Two Completely Different Meanings

This is the first trap. Same symbol, two meanings depending on context.

### In a DECLARATION:

```c
int *p;
```

Here `*` does NOT mean dereference. It means:
**"p is a pointer type — it will hold an address of an int"**

It is telling the compiler what KIND of variable p is.

```c
int    *p;   // p holds address of an int
char   *p;   // p holds address of a char
double *p;   // p holds address of a double
int   **p;   // p holds address of a pointer to int
```

### In USAGE:

```c
*p
```

Here `*` means **dereference** — follow the address stored in p, go there, read the value.

```c
int a = 10;
int *p = &a;

*p   // go to 0x00000021, read what's there → 10
```

---

## 3. The & Symbol

`&` means **"give me the address of"**. Always. No exceptions.

```c
int a = 10;

&a    // address of a       → 0x00000021
&p    // address of p       → 0x00000033  (p is a variable too, it has an address)
```

---

## 4. The Complete Reference Table

```c
int a = 10;
int *p = &a;
```

```
Expression   │ Meaning                              │ Value
─────────────┼──────────────────────────────────────┼──────────────────
a            │ value of a                           │ 10
&a           │ address of a                         │ 0x00000021
p            │ value of p (address it holds)        │ 0x00000021
&p           │ address of p itself                  │ 0x00000033
*p           │ value at address p holds             │ 10
```

---

## 5. Modifying Through a Pointer

```c
int a = 10;
int *p = &a;

*p = 50;
```

`*p = 50` means: go to address `0x00000021`, write `50` there.

```
BEFORE:                          AFTER *p = 50:
        a                               a
   ┌─────────┐                     ┌─────────┐
   │   10    │                     │   50    │  ← changed
   └─────────┘                     └─────────┘
   0x00000021                      0x00000021

        p                               p
   ┌─────────────────────┐         ┌─────────────────────┐
   │     0x00000021       │         │     0x00000021       │  ← unchanged
   └─────────────────────┘         └─────────────────────┘
        0x00000033                       0x00000033
```

The address stored in `p` does NOT change. Only the VALUE at that address changes.

To change the address stored in p:

```c
int b = 20;
p = &b;    // now p holds 0x00000025 instead of 0x00000021
```

---

## 6. Pointer Arithmetic — Addresses Scale by Type Size

```c
int a = 10;
int *p = &a;

p++;
```

`p++` does NOT add 1 to the address. It adds **sizeof(int) = 4**.

```
Before:  p = 0x00000021
After:   p = 0x00000025   (moved 4 bytes forward, not 1)
```

This is **pointer arithmetic**. The compiler automatically scales by the pointed-to type:

```c
int    *p;  p++;   // moves 4 bytes  (sizeof int)
char   *p;  p++;   // moves 1 byte   (sizeof char)
double *p;  p++;   // moves 8 bytes  (sizeof double)
```

So `p + 3` for an `int *p`:

```
p + 3 = current_address + (3 × 4) = current_address + 12
```

This is exactly how arrays work internally. `arr[i]` is literally `*(arr + i)`.

---

## 7. The *p++ Precedence Trap

```c
int a = 10;
int *p = &a;

*p++    // what does this do?
```

**`++` binds tighter than `*` (higher precedence).**

So it is parsed as `*(p++)` not `(*p)++`.

Order of operations:
1. Read current value of `*p` → 10
2. Increment `p` itself (move pointer forward 4 bytes)
3. Return the value that was read (10)

To increment the VALUE at the address:

```c
(*p)++   // parentheses force dereference first, then increment value
          // a becomes 11, p stays at same address
```

```
*p++   → reads value, moves pointer    (pointer arithmetic)
(*p)++ → increments value at address   (value modification)
```

This is one of the most common C bugs. One character difference, completely different behavior.

---

## 8. sizeof on Pointers

```c
int a = 10;
int *p = &a;

sizeof(p)    // 8 — pointer, always 8 bytes on x64
sizeof(*p)   // 4 — int, 4 bytes
sizeof(&p)   // 8 — address of p, still a pointer, still 8 bytes
```

**All pointers are 8 bytes on x86-64. Always. Regardless of type.**

```c
int    *p;   sizeof(p) = 8
char   *p;   sizeof(p) = 8
double *p;   sizeof(p) = 8
int   **p;   sizeof(p) = 8
void   *p;   sizeof(p) = 8
```

The type only tells the compiler how to interpret what's at that address.
The pointer itself is always 8 bytes.

Use `%zu` not `%d` for sizeof (returns `size_t`, not `int`).

---

## 9. Pointer to Pointer — **pp

```c
int a = 10;
int *p = &a;
int **pp = &p;
```

```
        a                   p                    pp
   ┌─────────┐         ┌──────────┐         ┌──────────┐
   │   10    │         │0x00000021│         │0x00000033│
   └─────────┘         └──────────┘         └──────────┘
   0x00000021           0x00000033           0x00000044
```

Dereferencing chain:

```
**pp  →  *(0x00000033)  →  0x00000021  →  *(0x00000021)  →  10
 *pp  →  *(0x00000044)  →  0x00000033  →  value of p (which is address of a)
  pp  →  0x00000044     →  address of pp itself
```

### Changing what p points to through pp:

```c
int b = 20;
*pp = &b;
```

`*pp` = value at address `0x00000044` = `p`'s storage location.
Writing `&b` there changes what `p` holds.

```
AFTER *pp = &b:

        a                   p                    pp
   ┌─────────┐         ┌──────────┐         ┌──────────┐
   │   10    │         │0x00000025│  ←changed │0x00000033│
   └─────────┘         └──────────┘         └──────────┘
   0x00000021           0x00000033           0x00000044

        b
   ┌─────────┐
   │   20    │
   └──────── ┘
   0x00000025
```

`p` now points to `b`. `a` is untouched.
`*p` now gives `20`.

### The difference:

```c
**pp = 99    // change value of a (go two levels deep, write value)
 *pp = &b    // change what p points to (go one level, write address)
  pp = &x    // change what pp points to (change pp itself)
```

---

## 10. Passing by Reference — The Core Pattern

In C, everything is **passed by value** by default. A copy is made.

```c
void try_change(int x) {
    x = 100;    // modifies the COPY, not original
}

int main(void) {
    int a = 10;
    try_change(a);
    printf("%d\n", a);   // still 10
}
```

To actually modify the original — pass the address:

```c
void actually_change(int *p) {
    *p = 100;    // modifies what p points to — the original
}

int main(void) {
    int a = 10;
    actually_change(&a);
    printf("%d\n", a);   // 100
}
```

```
main stack:              actually_change stack:
┌──────────┐             ┌──────────┐
│  a = 10  │ ←0x0021     │  p=0x0021│
└──────────┘             └──────────┘
      ↑                        │
      └────────────────────────┘
                *p = 100 writes here
```

`p` dies when the function returns. `a` stays. The modification persists.

`scanf("%d", &a)` is exactly this pattern — scanf receives your address,
writes the parsed value there, returns. The & is mandatory for this reason.

---

## 11. The Three Dangerous Pointer States

### 11.1 Uninitialized Pointer

```c
int *p;       // p declared but never assigned
*p = 50;      // writing to random garbage address
```

`p` holds whatever garbage was on the stack at that memory location.
Could be any address. Writing there corrupts random memory.

**Does not always crash immediately.** May silently corrupt memory and crash
somewhere completely unrelated later. Hardest bug to find.

With `-Wall`:
```
warning: 'p' is used uninitialized
```

### 11.2 NULL Pointer

```c
int *p = NULL;   // explicitly points to address 0x0000000000000000
*p = 50;         // SIGSEGV — immediate crash
```

NULL = address `0x0000000000000000`. The OS deliberately leaves the first page
(`0x0000` to `0x0FFF`) unmapped. No process can access it.

MMU checks page table → no valid mapping → page fault → kernel sends SIGSEGV →
process dies immediately.

**Crash is loud and immediate.** This is SAFER than uninitialized.
You know exactly where it broke.

Always initialize pointers to NULL if you don't have an address yet:
```c
int *p = NULL;
if (p != NULL) {
    *p = 50;    // safe
}
```

### 11.3 Dangling Pointer

```c
int *give_address(void) {
    int x = 100;    // x lives on stack of this function
    return &x;      // returning address of local variable
}

int main(void) {
    int *p = give_address();
    printf("%d\n", *p);    // UB — x is gone, stack frame deallocated
}
```

When `give_address` returns, its stack frame is gone. The address `p` holds
is now pointing to deallocated stack memory.

```
DURING function:          AFTER function returns:
┌──────────┐              ┌──────────┐
│  x = 100 │  0x0022      │ garbage  │  0x0022  ← same address, invalid data
└──────────┘              └──────────┘
      ↑                         ↑
      p                         p  ← DANGLING
```

GCC warns:
```
warning: function returns address of local variable
```

### The Difference:

```
Uninitialized  → never had valid address, garbage from stack
NULL           → explicitly invalid, crashes loudly on access
Dangling       → used to be valid, no longer valid, silent corruption
```

Dangling is the most dangerous. NULL is the safest failure mode.

---

## 12. Two Pointers, Same Address — Aliasing

```c
int a = 10;
int *p = &a;
int *q = &a;   // both point to same location

*p = 50;

printf("%d\n", *q);   // 50 — q sees the change made through p
```

```
p ──→ ┌──────┐ ←── q
      │  50  │
      └──────┘
       0x0021
```

Both pointers see the same memory. Modify through one, the other sees it.

**This is exactly what Rust prevents with the borrow checker:**

```rust
let mut a = 10;
let p = &mut a;
let q = &mut a;   // ERROR: cannot have two mutable references
```

In C — no error, no warning. This is called **aliasing** and causes some of the
hardest bugs in real C codebases.

---

## 13. Format Specifier Mismatch with Pointers

```c
int a = 10;
int *p = &a;

printf("%d\n", p);    // WRONG — %d reads 4 bytes, pointer is 8 bytes
printf("%p\n", p);    // CORRECT — prints full address in hex
printf("%lu\n", p);   // also valid — address as decimal
```

`%d` reads 4 bytes. A pointer on x64 is 8 bytes. So `%d` reads only the lower
4 bytes of the address and interprets them as a signed integer. Garbage output.

```
Full pointer (8 bytes):  0x00007FFD8A3B1C20
%d reads lower 4 bytes:  0x8A3B1C20  →  interpreted as signed int → large negative number
```

Compiler with `-Wall` catches this:
```
warning: format '%d' expects argument of type 'int' but argument has type 'int *'
```

---

## 14. The Virtual Address Space — Where Everything Lives

```
Virtual Address Space of your process (x86-64 Linux):

0xFFFFFFFFFFFFFFFF  ┌────────────────────┐
                    │   KERNEL SPACE     │  ring 0 — kernel only
                    │                    │  user process cannot touch this
0xFFFF800000000000  ├────────────────────┤  ← hard boundary
                    │                    │
                    │      STACK         │  rw-  grows downward
                    │   (local vars,     │  auto-managed
                    │    function calls) │
                    ├────────────────────┤
                    │       ↕            │  (gap — ASLR randomizes positions)
                    ├────────────────────┤
                    │      HEAP          │  rw-  grows upward
                    │   (malloc, etc)    │  manually managed
                    ├────────────────────┤
                    │      libc.so.6     │  r-x  (printf, scanf live here)
                    ├────────────────────┤
                    │      .text         │  r-x  your compiled code
                    │      .rodata       │  r--  string literals
                    │      .data         │  rw-  initialized globals
                    │      .bss          │  rw-  uninitialized globals (zeroed)
                    ├────────────────────┤
                    │    UNMAPPED        │  ---  deliberately empty
                    │    (NULL zone)     │       first page, no mapping
0x0000000000000000  └────────────────────┘
```

**ASLR** (Address Space Layout Randomization) — randomizes WHERE the stack,
heap, and libraries are placed every time the program runs. The layout above
is the structure, but the exact addresses shift each run.

**Kernel zeroing** — when the OS allocates fresh memory pages to your process,
it zeros them first (security — previous process data must not leak).
This is why uninitialized local variables often read as 0 on Linux.
Not guaranteed. UB. But that's the mechanism.

**NULL at 0x0000** — the very first page is unmapped deliberately.
Any access (read or write) to this region → page fault → SIGSEGV.

---

## 15. Why NULL Crashes but Uninitialized Might Not

```
Uninitialized pointer:
p = some garbage value = 0x00007ffd8a3b1c20
    → this IS within the stack region
    → stack IS mapped (rw-)
    → MMU finds a valid page
    → access succeeds (reads/writes garbage, but no crash)
    → silent memory corruption

NULL pointer:
p = 0x0000000000000000
    → this is in the UNMAPPED region
    → no page table entry exists
    → MMU raises page fault
    → kernel sends SIGSEGV
    → immediate crash
```

Same reason. Different region. One mapped, one not.

---

## 16. The SIGSEGV Mechanism — Full Chain

When you dereference a bad pointer:

```
*p  (where p = invalid address)
↓
CPU issues memory read instruction for that address
↓
MMU walks the page table looking for a mapping
↓
No valid mapping found (or mapping is read-only and you're writing)
↓
MMU raises a page fault exception
↓
CPU transfers control to kernel's page fault handler
↓
Kernel checks: can this fault be resolved? (No — bad address)
↓
Kernel sends signal 11 (SIGSEGV) to your process
↓
Default SIGSEGV handler terminates the process
↓
Shell sees non-zero exit code, fish reports the signal name
```

**SIGSEGV** = Segmentation Violation. Name comes from old memory model where
address space was divided into "segments." Accessing outside valid segments =
segmentation violation. Modern x86-64 uses page tables not segments, but the
signal name remains.

---

## 17. Kernel Space — What Happens if You Could Write There

The virtual address space is split:

```
User space  (ring 3) — your program runs here
Kernel space (ring 0) — OS kernel runs here
```

The CPU enforces this in hardware. User space attempting to access kernel space
→ immediate SIGSEGV.

**But if a vulnerability allows writing to kernel space:**

- Overwrite kernel code → execute arbitrary code at ring 0 → full system compromise
- Overwrite kernel data structures → escalate privileges from user to root
- Modify page tables → remap any memory however you want

Ring 0 has no restrictions. No SIGSEGV, no permission checks. If your code runs
there — you own the entire machine. Not just your process. Every process.

This is why kernel CVEs score 9.8-10.0. One bug = full system takeover.

Example: **Dirty COW (CVE-2016-5195)** — race condition in the kernel's
copy-on-write mechanism. Allowed writing to read-only memory. Lived in the
kernel for **9 years** undetected. Full root on any Linux system.

---

## 18. Common Pointer Bugs Summary

```
Bug                  │ Cause                          │ Symptom
─────────────────────┼────────────────────────────────┼────────────────────────
Uninitialized ptr    │ int *p; *p = x;                │ Silent corruption / crash
NULL dereference     │ int *p = NULL; *p = x;         │ SIGSEGV immediately
Dangling pointer     │ return &local_var;             │ Garbage / SIGSEGV later
Double free          │ free(p); free(p);              │ Heap corruption
Use after free       │ free(p); *p = x;               │ Silent corruption
Buffer overflow      │ writing past array end         │ Corruption / exploit
Aliasing             │ two mutable ptrs same address  │ Unexpected modifications
Type mismatch        │ printf("%d", ptr)              │ Garbage output / UB
*p++ vs (*p)++       │ precedence confusion           │ Wrong operation entirely
Missing & in scanf   │ scanf("%d", a) not &a          │ SIGSEGV on integer input
```

---

## 19. Rust vs C — The Same Bugs, Different Outcomes

```
Situation              │ C                      │ Rust
───────────────────────┼────────────────────────┼──────────────────────
Two mutable references │ Allowed, silent bug    │ Compile error
NULL dereference       │ SIGSEGV at runtime     │ Option<T>, no null
Dangling pointer       │ UB at runtime          │ Borrow checker prevents
Uninitialized variable │ UB, reads garbage      │ Compile error
Array out of bounds    │ Silent corruption      │ Runtime panic
Type size mismatch     │ Silent truncation      │ Compile error
Use after free         │ UB at runtime          │ Borrow checker prevents
```

Rust moves every C runtime disaster to compile time.
The borrow checker is essentially a formal proof that none of these bugs exist.

C trusts you completely. Rust does not.

---

## 20. The Complete Mental Model

```
Every variable:
┌──────────────────────────────────────────┐
│  name  │  type  │  value  │  address     │
│   a    │  int   │   10    │  0x00000021  │
└──────────────────────────────────────────┘

A pointer:
┌──────────────────────────────────────────┐
│  name  │  type  │  value      │ address  │
│   p    │  int*  │ 0x00000021  │ 0x000033 │
└──────────────────────────────────────────┘
                       ↑
                  stores address of a

Operations:
a          → value of a                    → 10
&a         → address of a                 → 0x00000021
p          → value of p (an address)      → 0x00000021
&p         → address of p                 → 0x00000033
*p         → value AT address p holds     → 10
*p = 50    → write 50 AT address p holds  → a becomes 50
p = &b     → change address p holds       → p now points to b
(*p)++     → increment value at address   → a becomes 11
*p++       → read value, move pointer     → reads a, p moves forward 4 bytes
```

---

## Compile Command (always use this)

```bash
gcc -Wall -Wextra -Wpedantic -std=c11 -o program program.c
```

`-Wall`     → all common warnings
`-Wextra`   → extra warnings  
`-Wpedantic`→ strict C standard enforcement
`-std=c11`  → C11 standard

These flags catch most pointer bugs at compile time before they become runtime disasters.
