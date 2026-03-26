## This is the explanation for the object dump from the code. 
- You can also read the web version of this explanation, below given is the detail explanation of the memory layout. 
- If you do not know about the registers, etc tech margon terms related to C, don't worry, all necessary definitions will be provided below.

Here's a program written to deliberately put something in **every** memory region, followed by the full table.

---

## The Program

```c
#include <stdio.h>
#include <stdlib.h>

// ── BSS: global, declared, NO value ──────────────────
int   global_uninit;           // BSS
int   global_arr[10];          // BSS (entire array zero-filled)

// ── DATA: global, declared, HAS value ────────────────
int   global_init   = 42;      // DATA
float global_float  = 3.14f;   // DATA
char  global_char   = 'A';     // DATA

// ── RODATA: constants, never change ──────────────────
const int  CONST_NUM = 100;          // RODATA
const char *literal  = "I am text";  // "I am text" → RODATA
                                     //  literal ptr itself → DATA

// ── TEXT: a function (compiled to machine code) ──────
int add(int a, int b) {        // TEXT
    return a + b;
}

int main(void) {

    // ── STACK: local variables ────────────────────────
    int   local_int        = 5;
    float local_float      = 2.3f;
    char  local_char       = 'Z';
    int   local_arr[3]     = {1, 2, 3};

    // ── HEAP: manually allocated ──────────────────────
    int *heap_ptr = malloc(sizeof(int) * 5);
    heap_ptr[0]   = 99;

    // Use everything (prevents compiler from optimizing away)
    printf("BSS   global_uninit : %d\n",  global_uninit);
    printf("DATA  global_init   : %d\n",  global_init);
    printf("DATA  global_float  : %.2f\n",global_float);
    printf("DATA  global_char   : %c\n",  global_char);
    printf("RODATA CONST_NUM    : %d\n",  CONST_NUM);
    printf("RODATA literal      : %s\n",  literal);
    printf("STACK local_int     : %d\n",  local_int);
    printf("STACK local_float   : %.2f\n",local_float);
    printf("STACK local_char    : %c\n",  local_char);
    printf("HEAP  heap_ptr[0]   : %d\n",  heap_ptr[0]);
    printf("TEXT  add(2,3)      : %d\n",  add(2, 3));

    free(heap_ptr);
    return 0;
}
```

---

## The Full Memory Layout Table

| # | Region / Section | rwx | What lives there from THIS program | Why it's there | Exists in file? |
|---|---|---|---|---|---|
| 1 | `.text` | `r-x` | `main()`, `add()`, `_start`, `_init`, `_fini` | Compiled machine code the CPU runs. Read+Execute, never writable — so code can't corrupt itself | ✅ Yes |
| 2 | `.rodata` | `r--` | `"I am text"` string, `CONST_NUM = 100`, `"BSS global_uninit..."` printf strings | Constants that never change. Read-only so nothing can accidentally modify them | ✅ Yes |
| 3 | `.data` | `rw-` | `global_init=42`, `global_float=3.14`, `global_char='A'`, the pointer `literal` | Global variables that have an initial value. Writable because globals can be changed at runtime | ✅ Yes |
| 4 | `.bss` | `rw-` | `global_uninit`, `global_arr[10]` | Global variables with NO initial value. OS zero-fills them. Takes zero space in the file — only the size is recorded | ❌ No (only size recorded) |
| 5 | `Stack` | `rw-` | `local_int`, `local_float`, `local_char`, `local_arr[3]`, `heap_ptr` (the pointer variable, not what it points to), return addresses, `argc`, `argv` | Temporary storage for everything inside functions. Automatically created and destroyed as functions are called/returned. Grows downward in memory | ❌ No (runtime only) |
| 6 | `Heap` | `rw-` | The 5 `int`s that `malloc` gave us (`heap_ptr[0..4]`) | Memory you manually request with `malloc`. Lives until you `free()` it. You control the lifetime — OS doesn't manage it | ❌ No (runtime only) |
| 7 | `.init` / `.fini` | `r-x` | C runtime setup before `main`, cleanup after `main` | Runs global constructors, initializes libc. You never see this code but it always runs | ✅ Yes |
| 8 | `.dynamic` | `rw-` | `NEEDED libc.so.6`, addresses of `INIT`, `FINI`, symbol table pointers | Tells the dynamic linker what shared libraries to load and where to find things | ✅ Yes |
| 9 | `.got` / `.got.plt` | `rw-` | Actual runtime addresses of `printf`, `malloc`, `free` from libc | At compile time the addresses of library functions are unknown. The linker fills these in at runtime. Writable for exactly this reason | ✅ Yes |
| 10 | `.interp` | `r--` | `/lib64/ld-linux-x86-64.so.2` (the dynamic linker path) | Tells the kernel which program to use to load shared libraries before your program starts | ✅ Yes |
| 11 | `.eh_frame` | `r--` | Stack unwinding tables | Used when exceptions or crashes happen — tells the system how to walk back up the call stack | ✅ Yes |
| 12 | `PHDR` | `r--` | The program header table itself | The map of all segments. The OS reads this first to understand how to load everything else | ✅ Yes |

---

## Permissions — Your rwx Intuition Applied

Since you already know `rwx` from Linux file permissions, this maps perfectly:

```
r-x  =  read + execute     → code regions (.text, .init, .fini)
r--  =  read only          → constants (.rodata, .interp, .eh_frame)
rw-  =  read + write       → data regions (.data, .bss, stack, heap, .got)
---  =  none               → doesn't exist (you'd segfault touching it)
```

The key security insight — **no region is `rwx` (all three)**. If code is executable, it's not writable. This is intentional: if a region were both writable AND executable, an attacker could write malicious code into it and execute it. This protection is called **NX bit** (No-Execute), and it's enforced by the CPU itself.

---

## Visual Memory Map (low → high address)

```
Low address  0x0000...
─────────────────────────────────────────────
0x0000       ELF headers / PHDR     [r--]
0x0374       .interp                [r--]  ← path to dynamic linker
─────────────────────────────────────────────
0x1000       .init                  [r-x]  ← runs before main
0x1020       .text  (_start, add,   [r-x]  ← YOUR compiled code
              main live here)
0x1124       .fini                  [r-x]  ← runs after main
─────────────────────────────────────────────
0x2000       .rodata                [r--]  ← "I am text", CONST_NUM
0x2004       .eh_frame              [r--]
─────────────────────────────────────────────
0x3e10       .init_array            [rw-]
0x3e20       .dynamic               [rw-]  ← libc linking info
0x3fe8       .got / .got.plt        [rw-]  ← printf/malloc addresses
0x4000       .data                  [rw-]  ← global_init, global_float
0x4010       .bss                   [rw-]  ← global_uninit, global_arr
─────────────────────────────────────────────
  ↕ big gap  (unmapped, segfault if touched)
─────────────────────────────────────────────
               HEAP                 [rw-]  ← malloc'd memory, grows ↑
               ...
               STACK                [rw-]  ← local vars, grows ↓
─────────────────────────────────────────────
High address 0xFFFF...
```

The **stack grows downward** (from high to low), the **heap grows upward** (from low to high). They grow toward each other. If they ever meet — that's a stack overflow or out-of-memory.

## Me

But I am sure that the above memory layout is not exactly as it is in the standard form. The above layout is just to demonstrate what is there in the memory layout and how it looks like. But NOTE that this is not the exact order. See my scanf's last example to experience a segfault, in the errors dir. Soon, I will be able to pull up the original memory stack layout, from online resources available, in a standard form lol. Now to remember that just : stack grows downward, automatic memory allocation. Heap grows upward, manual memory allocation using malloc. Yet, I guess this frame layout is reversed. But it is correct for sure. It is nice to visualise it from High to low address. High address usually contains the kernel space. 


