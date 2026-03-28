# The `%p` and `void *` Rabbit Hole — A Deep Dive

## 1. What Is `%p` Actually Expecting?

The C standard (C23, §7.23.6.1) is unambiguous:

> *"The argument shall be a pointer to void."*

That's it. `%p` takes **exactly** `void *`. Anything else — `int *`, `float *`, `char *` — is **undefined behaviour** (UB), even if it works on every machine you've ever touched.

---

## 2. Why Is It Undefined Behaviour?

### 2.1 Variadic Functions and the Lack of Type Safety

`printf` is declared as:

```c
int printf(const char *fmt, ...);
```

The `...` means: *"I accept anything after `fmt`, and I make zero promises about types."*

When the compiler sees a normal function call like:

```c
void foo(void *p);
foo(my_int_ptr);   // compiler inserts implicit cast here
```

It **knows** the target type is `void *` and silently inserts the cast. Safe.

But with variadic args:

```c
printf("%p", my_int_ptr);  // compiler has NO type information here
```

The compiler only sees `...`. It **cannot** insert a cast because it doesn't know what type the callee expects. The raw bit pattern of `my_int_ptr` is pushed onto the stack (or into a register), and `printf` reads it back assuming it is a `void *`. If those two pointer types have different representations — the read is wrong.

### 2.2 The C Standard's Pointer Representation Rule

The C standard **does not** guarantee that all pointer types have the same size or representation. It only guarantees:

| Guarantee | Rule |
|-----------|------|
| `void *` ↔ `char *` | Same size & alignment (§6.2.5) |
| `void *` ↔ any object pointer | Conversion is safe both ways |
| `int *` ↔ `float *` | **No guarantee** — may differ |
| Function pointers | **Explicitly excluded** |

So `int *` and `void *` *might* be different sizes. On most modern 64-bit systems they happen to be the same (8 bytes), but the standard does not promise this.

---

## 3. Memory Layout: Where the Danger Lives

### 3.1 Normal 64-bit x86-64 Layout (Where It "Works")

```
Stack frame during printf("%p", my_int_ptr)
┌─────────────────────────────────────────┐  high address
│  ...other locals...                     │
├─────────────────────────────────────────┤
│  my_int_ptr  (int *)   8 bytes          │  ← pushed to stack / rsi register
│  0x00007ffd_a3b2c100                    │
├─────────────────────────────────────────┤
│  fmt ptr     (char *)  8 bytes          │  ← rdi register
│  0x00000000_00401234                    │
└─────────────────────────────────────────┘  low address

printf reads rsi as void * → gets 0x00007ffd_a3b2c100 ✓ (happens to match)
```

This *works* because on x86-64, `int *` and `void *` are both 64-bit pointers with identical representation. The UB is **latent** — silent but present.

### 3.2 A Hypothetical Platform Where It Breaks

Imagine a segmented memory architecture (like old 8086, or some DSPs):

```
int *   = { segment: 16 bits | offset: 16 bits }  = 32 bits total
void *  = { segment: 16 bits | offset: 32 bits }  = 48 bits total

Stack when int * is passed where void * is expected:
┌──────────────────────┐
│  int *   (32 bits)   │  0xABCD1234
│  ??garbage?? 16 bits │  0x????        ← printf reads 48 bits total!
└──────────────────────┘

printf gets: 0xABCD1234???? — completely wrong address
```

This is not hypothetical for embedded or DSP targets. It happens.

### 3.3 The `va_list` Internals

Under the System V AMD64 ABI, `va_list` is a struct:

```c
typedef struct {
    unsigned int gp_offset;   // offset into reg_save_area for GP regs
    unsigned int fp_offset;   // offset for FP regs
    void *overflow_arg_area;  // pointer to stack args
    void *reg_save_area;      // saved registers
} va_list[1];
```

`va_arg(ap, void *)` reads 8 bytes from `reg_save_area[gp_offset]` and interprets them as a `void *`. If you passed an `int *` that happens to be 8 bytes — it works. If you passed a platform where `int *` is 4 bytes — `va_arg` reads 8 bytes and half of it is garbage from an adjacent register or stack slot.

---

## 4. The Compiler's Perspective: Why Clang Warns

Clang's `-Wformat-pedantic` flag enforces **exact type matching** for format specifiers. It knows:

- `%p` → expects `void *`
- You gave `int *`
- These are *not the same type*
- Therefore: **warn**

Without `-pedantic`, many compilers stay silent because in practice (x86-64 Linux/macOS/Windows) they are the same size. But pedantic mode enforces the letter of the standard.

---

## 5. Is There a Security Bug Here?

### 5.1 Direct Vulnerability: Probably Not in This Case

For `printf("%p", some_ptr)`, the worst realistic outcome on x86-64 is printing a wrong address or garbage. That's a **correctness bug**, not typically an exploitable security bug by itself.

### 5.2 Indirect Vulnerability: Absolutely Yes

The *mechanism* behind this UB — wrong data being read from the wrong location via `va_list` — is the exact same mechanism behind real CVEs. Consider:

**Format String Vulnerabilities (classic exploit class):**

```c
// Attacker controls fmt
char *fmt = get_user_input();
printf(fmt);  // NO second argument — catastrophic
```

With a crafted format string like `"%p %p %p %p %p %p %p %08x"`, an attacker can:

1. **Read stack memory** — leak addresses, canary values, return addresses
2. **Defeat ASLR** — by leaking a known address, compute the base of all loaded libraries
3. **With `%n`** — write arbitrary values to arbitrary addresses → arbitrary code execution

This is CVE-class material. The `printf` family is historically one of the most exploited interfaces in C.

### 5.3 UB as an Attack Surface More Broadly

When a program has undefined behaviour, the compiler is allowed to **optimize it away or transform it** in surprising ways. A UB pointer cast can cause:

- **Dead code elimination** — a null-check the compiler "knows" is always false gets removed
- **Aliasing violations** — the compiler reorders memory accesses, causing data races or wrong reads
- **Type confusion** — reading memory as the wrong type, potentially leaking sensitive data

---

## 6. Exploit Demo: Leaking Stack Memory via `%p`

This program deliberately demonstrates two things:

1. Passing a typed pointer without casting (the original warning)
2. A classic format-string stack read using multiple `%p` specifiers

```c
/*
 * exploit_demo.c
 *
 * Demonstrates:
 *   (a) UB from passing typed pointer to %p without void* cast
 *   (b) Stack memory leak via format string with no argument
 *
 * Compile:
 *   clang -o exploit_demo exploit_demo.c -std=c23 -g -fno-stack-protector
 *   (Note: -Wformat-pedantic will warn on the UB lines — intentional)
 *
 * WARNING: Educational use only. Never disable stack protector in production.
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>

// Secret data we want to "protect"
static const char secret[] = "SECRET_KEY=hunter2";

void demonstrate_ub_cast(void) {
    int   a     = 0xDEADBEEF;
    float b     = 3.14f;
    double k    = 2.718281828;

    int    *p_int    = &a;
    float  *p_float  = &b;
    double *p_double = &k;

    puts("=== (A) UB: passing typed pointer to %p without cast ===");
    puts("    (Clang warns: format specifies void* but got int*)");
    puts("");

    // ❌ UB — no cast. Works on x86-64 by accident.
    printf("  int*    without cast: %p\n", p_int);
    printf("  float*  without cast: %p\n", p_float);
    printf("  double* without cast: %p\n", p_double);

    puts("");
    puts("    The correct way — explicit cast to void*:");

    // ✅ Correct
    printf("  int*    with cast:    %p\n", (void *)p_int);
    printf("  float*  with cast:    %p\n", (void *)p_float);
    printf("  double* with cast:    %p\n", (void *)p_double);
}

void vulnerable_print(char *user_input) {
    // ❌ NEVER do this — user controls the format string
    printf(user_input);
    putchar('\n');
}

void demonstrate_stack_leak(void) {
    // Put something interesting on the stack near vulnerable_print
    char local_buffer[32] = "LOCAL_STACK_DATA_HERE";
    uint64_t canary_sim   = 0xCAFEBABEDEADC0DE;

    (void)local_buffer; // suppress unused warning
    (void)canary_sim;

    puts("\n=== (B) Format string stack leak ===");
    puts("    Attacker sends: \"%p %p %p %p %p %p %p %p\"");
    puts("    (Each %p reads 8 bytes from the stack/registers)");
    puts("");

    // Simulate attacker-controlled input
    char attacker_payload[] = "%p %p %p %p %p %p %p %p %p %p";

    printf("  Stack dump: ");
    vulnerable_print(attacker_payload);

    puts("");
    puts("    ^ These are raw stack addresses and values.");
    puts("    ^ With symbols/maps, attacker can locate secret[], return addrs, etc.");
    puts("    ^ 'secret' is at a known offset from these leaked pointers.");
}

void show_memory_layout(void) {
    int    i = 42;
    float  f = 42.0f;
    double d = 42.0;
    void  *vp;

    puts("\n=== (C) Memory layout: pointer sizes on this platform ===");
    printf("  sizeof(int *)    = %zu bytes\n", sizeof(int *));
    printf("  sizeof(float *)  = %zu bytes\n", sizeof(float *));
    printf("  sizeof(double *) = %zu bytes\n", sizeof(double *));
    printf("  sizeof(void *)   = %zu bytes\n", sizeof(void *));

    puts("");
    puts("  On x86-64: all equal → UB is silent.");
    puts("  On segmented/DSP arch: they may differ → UB causes wrong output or crash.");

    puts("\n  Byte representation of int * vs void *:");

    int *ip = &i;
    vp = (void *)ip;

    unsigned char *raw_ip = (unsigned char *)&ip;
    unsigned char *raw_vp = (unsigned char *)&vp;

    printf("  int * bytes:  ");
    for (size_t j = 0; j < sizeof(ip); j++) printf("%02x ", raw_ip[j]);
    puts("");

    printf("  void* bytes:  ");
    for (size_t j = 0; j < sizeof(vp); j++) printf("%02x ", raw_vp[j]);
    puts("");
    puts("  (Identical on this platform — the UB is hidden)");
}

int main(void) {
    // Print secret location so we can verify the leak
    printf("\n[secret[] lives at: %p]\n\n", (void *)secret);

    demonstrate_ub_cast();
    show_memory_layout();
    demonstrate_stack_leak();

    puts("\n=== Summary ===");
    puts("  1. Always cast to (void*) when passing pointers to %p");
    puts("  2. NEVER pass user input as the format string to printf");
    puts("  3. UB is silent on x86-64 but lethal on other platforms");
    puts("  4. Format string bugs are a real CVE class — not academic");

    return 0;
}
```

### Build & Run

```bash
# Build — intentionally no stack protector to show raw stack
clang -o exploit_demo exploit_demo.c -std=c23 -g -fno-stack-protector

# Run
./exploit_demo
```

### What You'll See

The `%p %p %p %p...` section will dump raw addresses from your stack. On a real vulnerable program, an attacker maps those addresses against the binary's symbol table to locate the return address, defeat ASLR, and craft a ROP chain or overwrite a function pointer.

---

## 7. The Fix: Always Cast

```c
// ❌ UB — never do this
printf("%p", my_int_ptr);

// ✅ Correct — explicit cast tells the compiler and reader what's happening
printf("%p", (void *)my_int_ptr);
```

The cast is zero-cost at runtime on all real platforms. It costs you exactly 8 characters. There is no reason not to do it.

---

## 8. Quick Reference: Pointer Conversion Rules in C

| Conversion | Legal? | Implicit? | Notes |
|---|---|---|---|
| `T *` → `void *` | ✅ Yes | ✅ Yes (in assignments) | ❌ No in variadic args |
| `void *` → `T *` | ✅ Yes | ✅ Yes (in assignments) | Requires explicit cast in C++ |
| `int *` → `float *` | ⚠️ Aliasing violation | ❌ No | UB unless through `memcpy` |
| `T *` → `uintptr_t` | ✅ Yes | ❌ No | Round-trip safe |
| Function ptr → `void *` | ❌ No | ❌ No | Undefined in standard C |

---

## 9. TL;DR

| Question | Answer |
|---|---|
| Why does clang warn? | `%p` requires `void *`; you passed `int *`; variadic args can't auto-cast |
| Is it UB? | Yes. The C standard explicitly requires `void *` for `%p` |
| Does it crash on x86-64? | Usually no — all pointers are 8 bytes there |
| Does it matter? | Yes — on other platforms it can corrupt output or crash |
| Is there a security bug? | Indirectly yes — same mechanism as format string exploits (real CVE class) |
| The fix? | Always `(void *)` cast before passing any pointer to `%p` |

