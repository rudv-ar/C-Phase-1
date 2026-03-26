# Explanation of the Memory Layout

# Memory Layout Explanation — hello_world
### Source: `hello_world.c`
```c
#include <stdio.h>

int main(void) {
  printf("helloworld!\n");
}
```
This is the program. Three lines of actual logic. And yet the binary that comes out of gcc is a machine with 26 sections, a dynamic linker, C runtime bootstrapping code, relocation tables, and symbol resolution infrastructure. This file explains all of it, top to bottom, in the exact order it appears in the objdump.

---

## Before anything else — what is RAM?

Think of RAM (memory) as a giant strip of numbered boxes. Each box holds 1 byte. Every box has an address — a number like `0x1040`. When your program runs, the operating system carves out a region of these boxes and hands them to your program. Your program then uses different parts of that region for different purposes — code goes here, strings go there, variables go somewhere else.

The objdump is a map of how your compiled binary is organized — both on disk (the file) and in memory (when it runs).

---

## What is an address?

An address is just a number. It refers to a specific byte in memory. All the `0x0000000000001040`-style numbers in this file are addresses. They are written in hexadecimal (base 16) because it's more compact than decimal for large numbers. `0x1040` in decimal is `4160`. It means: byte number 4160 in the program's memory space.

---

## Line 1 — File Format

```
hello_world:     file format elf64-x86-64
```

**ELF** stands for Executable and Linkable Format. It is the standard file format for executables on Linux (similar to `.exe` on Windows). Think of ELF as a very structured container — like a ZIP file, but for programs. It has a header describing what's inside, and then sections containing the actual data.

**64** means this is a 64-bit binary — addresses are 8 bytes wide (hence all the leading zeros in `0x0000000000001040`).

**x86-64** means it contains machine code instructions for the Intel/AMD 64-bit CPU architecture. If you tried to run this on an ARM chip (like an Apple M1), it would not work.

---

## Line 2 — Architecture and Flags

```
architecture: i386:x86-64, flags 0x00000150:
HAS_SYMS, DYNAMIC, D_PAGED
```

**flags** are metadata bits baked into the ELF header describing properties of this binary.

- **HAS_SYMS** — This binary still has its symbol table. Symbols are human-readable names for things (functions, variables). `main`, `puts`, `_start` are symbols. If you strip a binary (`strip hello_world`), this flag goes away and debugging becomes much harder.
- **DYNAMIC** — This binary is dynamically linked. It does not contain `printf`/`puts` code inside itself. Instead it says "go find `libc.so.6` at runtime and get `puts` from there." The alternative is static linking, where everything is baked in.
- **D_PAGED** — The binary is page-aligned. Memory is managed by the OS in chunks called pages (typically 4096 bytes = `0x1000`). This flag means the binary's segments are laid out respecting those boundaries so the OS can load them efficiently.

---

## Line 3 — Start Address

```
start address 0x0000000000001040
```

This is the very first instruction the CPU will execute when you run `./hello_world`. It is NOT `main`. It is `_start` — a function injected by the C runtime (glibc). `_start` does setup work (initializes the stack, environment variables, etc.) and then eventually calls your `main`. You wrote `main`. The system wrote `_start`.

---

## PROGRAM HEADER

The Program Header is a table that tells the **operating system** how to load this binary into memory. The OS does not care about sections (that's a linker/developer concept). It only cares about **segments** — big loadable chunks. Each row in the Program Header describes one segment.

Think of it this way:
- **Sections** = how the linker organizes things for building
- **Segments** = how the OS organizes things for running

Each segment entry has these fields:
- **off** — offset inside the file on disk (where this chunk starts in the `.elf` file)
- **vaddr** — virtual address in memory (where the OS puts this chunk when running)
- **paddr** — physical address (almost always same as vaddr on modern systems, ignored)
- **filesz** — how many bytes to read from the file
- **memsz** — how many bytes to reserve in memory (can be larger than filesz — the extra is zeroed, used for `.bss`)
- **flags** — permissions: `r` = readable, `w` = writable, `x` = executable

---

### PHDR

```
PHDR off 0x40 vaddr 0x40 flags r--
```

PHDR stands for "Program Header itself." This segment describes where the program header table lives. It is self-referential — the program header has an entry pointing to itself. The dynamic linker uses this to find and parse the rest of the header. Size is `0x310` = 784 bytes. Read-only (`r--`), never written or executed.

---

### INTERP

```
INTERP off 0x374 vaddr 0x374 flags r--  filesz 0x1c
```

INTERP = Interpreter. This tiny 28-byte (`0x1c`) segment contains a single null-terminated string: the path to the **dynamic linker**, which on Arch Linux is `/lib64/ld-linux-x86-64.so.2`.

When the kernel loads your binary, it reads this segment first. It says: "before running this program, run THIS program first, and hand it control." The dynamic linker then loads `libc.so.6`, resolves `puts`, patches the addresses, and only then jumps to `_start`. Without this segment, dynamic linking would not work at all.

Read-only (`r--`) — it's just a path string, nothing executes here.

---

### LOAD #1 — Read-only metadata

```
LOAD off 0x0 vaddr 0x0 filesz 0x600 flags r--
```

The first LOAD segment. It covers everything from the very beginning of the file up to `0x600` = 1536 bytes. This includes the ELF header, the program header table, and all the early read-only metadata sections (symbol tables, dynamic linking info, etc.).

Flags: `r--` — readable only. The CPU cannot execute this. You cannot write to it. It's just reference data.

The OS maps this directly from disk into memory. No copying — it's a memory-mapped file.

---

### LOAD #2 — Executable code

```
LOAD off 0x1000 vaddr 0x1000 filesz 0x161 flags r-x
```

This is the most important segment. It is mapped at address `0x1000` and it contains all executable machine code: `.init`, `.plt`, `.text`, `.fini`. This is where the actual CPU instructions live.

Flags: `r-x` — readable and executable, but NOT writable. This is a critical security feature. If this segment were writable, an attacker could overwrite your code at runtime (a code injection attack). The OS enforces this at the hardware level — any attempt to write here causes a segfault.

Size is only `0x161` = 353 bytes. Your entire `main` function is 26 of those bytes.

---

### LOAD #3 — Read-only data

```
LOAD off 0x2000 vaddr 0x2000 filesz 0x118 flags r--
```

This segment contains read-only data — mainly `.rodata` where string literals live. `"helloworld!\n"` is in here. It's readable but not writable and not executable.

Why can't you modify a string literal in C? Because it lives here, in a `r--` segment. Try `char *s = "hello"; s[0] = 'X';` — you'll get a segfault, because the OS rejects the write at the hardware level.

---

### LOAD #4 — Writable data

```
LOAD off 0x2dd0 vaddr 0x3dd0 filesz 0x248 memsz 0x250 flags rw-
```

This segment contains everything that needs to be both readable and writable at runtime: `.dynamic`, `.got`, `.got.plt`, `.data`, `.bss`.

Notice: `filesz 0x248` but `memsz 0x250` — the memory size is 8 bytes larger than the file size. Those extra 8 bytes are `.bss` — uninitialized global variables. They don't exist on disk (nothing to store yet), but the OS reserves zeroed memory for them at runtime.

Also notice: `vaddr 0x3dd0` but `off 0x2dd0` — on disk it's at `0x2dd0`, but in memory it's placed at `0x3dd0`. This offset exists to maintain alignment between segments while preserving the page-size boundaries the OS requires.

Flags: `rw-` — readable and writable, but NOT executable. This is another security feature (NX bit / DEP). Your variables live here, but the CPU will refuse to execute anything in this region. This prevents "data execution" attacks.

---

### DYNAMIC

```
DYNAMIC off 0x2de0 vaddr 0x3de0 flags rw-
```

This segment points directly to the `.dynamic` section, which is a structured list of key-value pairs the dynamic linker reads at startup. It contains entries like:
- "I need `libc.so.6`"
- "Here is where the symbol table is"
- "Here is where the relocation table is"

It overlaps with LOAD #4 — it's just pointing into the writable segment and saying "specifically, THIS part is the dynamic section."

---

### NOTE (three of them)

```
NOTE off 0x350 ...
NOTE off 0x20b8 ...
NOTE off 0x20f8 ...
```

NOTE segments carry metadata that tools (not the CPU) read. Three notes are present:

1. **`.note.gnu.build-id`** at `0x350` — A unique SHA1 hash fingerprint of this specific binary. Used by debuggers (like gdb) to find the matching debug symbols file even if the binary was stripped.

2. **`.note.gnu.property`** at `0x20b8` — CPU feature requirements. Tells the OS "this binary needs a CPU that supports these features." On modern Linux this includes things like Intel CET (Control-flow Enforcement Technology) — a hardware security feature.

3. **`.note.ABI-tag`** at `0x20f8` — Says "this binary requires at minimum Linux kernel version X.Y.Z." If you try to run it on an older kernel, the OS will reject it before even loading.

---

### 0x6474e553 (GNU_PROPERTY)

```
0x6474e553 off 0x20b8 ...
```

This is the same as the second NOTE above. `0x6474e553` is the numeric type identifier for `PT_GNU_PROPERTY`. Old versions of objdump display the raw number instead of the name. It's not a mysterious segment — it's just the GNU property note shown twice from different angles.

---

### EH_FRAME

```
EH_FRAME off 0x2010 vaddr 0x2010 flags r--
```

EH = Exception Handling. This segment points to `.eh_frame_hdr`, which is a lookup table for stack unwinding. When an exception or crash happens, the runtime needs to "unwind" the call stack — figure out what functions were called in what order to print a backtrace.

Even though C doesn't have exceptions, this data is still generated. It's used by C++ exception handling and also by tools like `gdb` and `perf` when they need to walk the call stack. Read-only.

---

### STACK

```
STACK off 0x0 vaddr 0x0 filesz 0x0 memsz 0x0 flags rw-
```

This segment is unusual: it has zero size and lives at address zero. It does not describe actual stack memory (the stack is allocated dynamically by the OS at runtime, not from the file). Its only purpose is to carry its **flags**: `rw-`.

The absence of `x` in the flags is the critical part. This tells the OS: "do not make the stack executable." This is the **NX (No-Execute) protection**. If the stack were executable, an attacker who overflowed a buffer and wrote shellcode there could execute it. With `rw-`, the CPU refuses to execute anything on the stack, which defeats a whole class of exploits.

---

### RELRO

```
RELRO off 0x2dd0 vaddr 0x3dd0 filesz 0x230 flags r--
```

RELRO = **RELocation Read-Only**. This is a hardening technique. Here's the problem it solves:

The `.got` and `.dynamic` tables (in the writable LOAD segment) contain pointers that the dynamic linker fills in at startup. After startup, those pointers never need to change. If an attacker overwrites them (via a buffer overflow), they can redirect function calls to malicious code.

RELRO says: after the dynamic linker finishes its work at startup, mark this region as read-only. The OS re-applies memory protections on the RELRO range, turning it from `rw-` to `r--`. The addresses are now locked.

This overlaps with LOAD #4 — same memory, but the OS applies extra protection after dynamic linking completes.

---

## DYNAMIC SECTION

This is the contents of the `.dynamic` section — a structured table the dynamic linker reads. Think of it as a config file embedded inside the binary.

```
NEEDED    libc.so.6
```
The single most important entry. It says: "this binary depends on `libc.so.6`." The dynamic linker finds this file (usually at `/usr/lib/libc.so.6`), loads it into memory, and makes its functions available. This is why `puts` works even though its code is not in your binary.

```
INIT      0x1000
FINI      0x1154
```
Addresses of the `.init` and `.fini` functions. The dynamic linker calls `.init` before `main` starts (for any global constructor setup) and `.fini` after `main` returns (for cleanup). You didn't write these — glibc injected them.

```
INIT_ARRAY   0x3dd0
INIT_ARRAYSZ 0x8
FINI_ARRAY   0x3dd8
FINI_ARRAYSZ 0x8
```
Arrays of function pointers to call before `main` (constructors) and after `main` (destructors). Each is 8 bytes = 1 pointer (64-bit). You have one of each, both injected by the C runtime.

```
GNU_HASH  0x390
SYMTAB    0x3b0
STRTAB    0x458
STRSZ     0x8d
SYMENT    0x18
```
Addresses of the dynamic symbol table and string table. The dynamic linker uses these to resolve symbol names at runtime. When it needs to find `puts`, it hashes the name, looks it up in `GNU_HASH`, finds the entry in `SYMTAB`, reads the name string from `STRTAB`, confirms it matches, then patches the address into `.got.plt`.

```
PLTGOT    0x3fe8
```
Address of the Global Offset Table (specifically the PLT portion). This is the patch target — where resolved function addresses get written.

```
PLTRELSZ  0x18
PLTREL    0x7
JMPREL    0x5e8
```
Location and size of the PLT relocation table. This tells the linker exactly which addresses in `.got.plt` need to be filled in and with which symbols.

```
RELA      0x528
RELASZ    0xc0
RELAENT   0x18
RELACOUNT 0x3
```
The general relocation table (not just for PLT). Contains 3 entries (`RELACOUNT`), each 24 bytes (`RELAENT` = `0x18`). These are addresses in the binary that need to be adjusted when the binary is loaded at its final memory address.

```
VERNEED   0x4f8
VERNEEDNUM 0x1
VERSYM    0x4e6
```
Version requirement tables. Used to ensure that the `libc.so.6` found at runtime is new enough. Connects to the Version References below.

```
FLAGS_1   0x8000000
```
The value `0x8000000` is the `PIE` flag — **Position Independent Executable**. This binary can be loaded at any memory address (ASLR). It does not require being at a fixed address. Every modern Linux binary compiled with `-fPIE` (the default on Arch) has this.

```
DEBUG     0x0
```
Reserved for the dynamic linker to write a pointer at runtime. Debuggers use it to find the linker's internal state. Zero on disk, patched in when running.

---

## VERSION REFERENCES

```
required from libc.so.6:
  0x09691a75  03  GLIBC_2.2.5
  0x069691b4  02  GLIBC_2.34
```

This says the binary requires two specific versions of glibc:

- **GLIBC_2.2.5** — needed for `puts`. This function has existed since glibc 2.2.5 (released ~2001). Your binary will work on almost any Linux system with glibc.
- **GLIBC_2.34** — needed for `__libc_start_main`. This is the function that calls `main`. Version 2.34 was released in 2021 and changed how it works. This is the binding constraint — your binary requires glibc 2.34 or newer to run.

The hex numbers (`0x09691a75`) are hash fingerprints of the version string. The dynamic linker verifies these hashes instead of comparing strings for speed.

---

## SECTIONS TABLE

If the Program Header was for the OS, the Sections table is for the developer and linker. It's a finer-grained breakdown of everything inside the binary. Each section has:

- **Idx** — index number
- **Name** — human-readable label
- **Size** — how many bytes
- **VMA** — virtual memory address (where it lives when running)
- **LMA** — load memory address (usually same as VMA)
- **File off** — where it starts in the file on disk
- **Algn** — alignment requirement (`2**4` means aligned to 16 bytes)
- **Flags line** — properties: CONTENTS (has data), ALLOC (gets memory), LOAD (copied from file), READONLY, CODE/DATA

---

### Section 0: `.note.gnu.build-id`
```
Size 0x24  VMA 0x350  READONLY DATA
```
24 bytes. Contains a unique SHA1 hash fingerprint of this exact compiled binary. If you recompile with different flags, the hash changes. Debuggers use this to match a stripped binary to its separate debug symbols file. Think of it as the binary's ID card.

---

### Section 1: `.interp`
```
Size 0x1c  VMA 0x374  READONLY DATA
```
28 bytes. Literally just the string `/lib64/ld-linux-x86-64.so.2` — the path to the dynamic linker. This is what causes the kernel to run the linker before your program. If you removed this section, the kernel would not know how to resolve `puts` and would refuse to run the binary.

---

### Section 2: `.gnu.hash`
```
Size 0x1c  VMA 0x390  READONLY DATA
```
A hash table over the dynamic symbols. The dynamic linker uses this to quickly find whether a given symbol name exists in this binary's exports. It uses the GNU hash algorithm (faster than the older ELF hash). Since `hello_world` doesn't export anything useful, this table is nearly empty.

---

### Section 3: `.dynsym`
```
Size 0xa8  VMA 0x3b0  READONLY DATA
```
Dynamic symbol table. `0xa8` = 168 bytes. Each symbol entry is 24 bytes (`SYMENT 0x18`), so this holds 7 dynamic symbols. These are the symbols the dynamic linker needs to know about — primarily `puts` (imported from libc) and a few glibc bookkeeping symbols.

---

### Section 4: `.dynstr`
```
Size 0x8d  VMA 0x458  READONLY DATA
```
Dynamic string table. `0x8d` = 141 bytes. This is where the actual name strings for `.dynsym` entries live. Symbols in `.dynsym` don't store the name directly — they store an offset into `.dynstr`. This indirection saves space when many symbols share common name prefixes.

---

### Section 5: `.gnu.version`
```
Size 0xe  VMA 0x4e6  READONLY DATA
```
14 bytes. One 2-byte version index per dynamic symbol entry (7 symbols × 2 bytes). Maps each symbol to a version requirement — connecting entries in `.dynsym` to version strings like `GLIBC_2.34`.

---

### Section 6: `.gnu.version_r`
```
Size 0x30  VMA 0x4f8  READONLY DATA
```
48 bytes. The "version needed" table. Lists which external libraries are needed and which specific version of each. This is what produced the "Version References" output above — both the `GLIBC_2.2.5` and `GLIBC_2.34` requirements come from here.

---

### Section 7: `.rela.dyn`
```
Size 0xc0  VMA 0x528  READONLY DATA
```
192 bytes = 8 relocation entries (each 24 bytes). These are general relocations — addresses in the binary that need to be adjusted when the binary is loaded. In a Position Independent Executable (PIE), nothing can assume it lives at a fixed address. Every absolute address reference needs a relocation entry so the loader can patch the correct final address in at load time.

---

### Section 8: `.rela.plt`
```
Size 0x18  VMA 0x5e8  READONLY DATA
```
24 bytes = 1 relocation entry. This single entry says: "patch this slot in `.got.plt` with the real address of `puts` once the dynamic linker finds it." This is how lazy binding works — the slot starts pointing to the resolver, and after the first call to `puts`, it gets overwritten with the real `puts` address so future calls skip the resolver entirely.

---

### Section 9: `.init`
```
Size 0x1b  VMA 0x1000  CODE
```
27 bytes of executable code. This runs before `main`. It's generated by gcc/glibc and performs low-level C runtime initialization. It calls things that need to happen before any of your code runs. You never call this — the dynamic linker calls it.

---

### Section 10: `.plt`
```
Size 0x20  VMA 0x1020  CODE
```
32 bytes. The **Procedure Linkage Table** — one of the most important mechanisms in dynamic linking.

Here's the problem it solves: your code calls `puts`. But `puts` lives in `libc.so.6`, which is loaded at some unknown address at runtime. Your binary cannot hardcode the address of `puts`. So instead, you call a stub in `.plt`.

The PLT stub does this:
1. Look up the address in `.got.plt`
2. If the address is already resolved (not the first call), jump there directly
3. If not resolved yet (first call), jump to the dynamic linker resolver, which finds `puts`, writes its address into `.got.plt`, then jumps to `puts`

After the first call, `.got.plt` has the real address and all future calls go direct. This is called **lazy binding** — the symbol is only resolved on first use.

32 bytes for 1 external function (`puts`) + the PLT header (16 bytes). Each PLT stub is 16 bytes.

---

### Section 11: `.text`
```
Size 0x113  VMA 0x1040  CODE
```
275 bytes. This is THE section. All real executable code lives here: `_start`, the CRT helper functions, and your `main`. Executable and readable but NOT writable.

The VMA `0x1040` matches the "start address" from the top of the objdump — confirming `_start` is at the very beginning of `.text`.

Your `main` is at `0x1139` (from the symbol table) and is `0x1a` = 26 bytes long. Twenty-six bytes. That's what `printf("helloworld!\n")` compiles to.

---

### Section 12: `.fini`
```
Size 0xd  VMA 0x1154  CODE
```
13 bytes. Mirror of `.init` — runs after `main` returns. Handles C runtime teardown. Also not written by you.

---

### Section 13: `.rodata`
```
Size 0x10  VMA 0x2000  READONLY DATA
```
16 bytes. Read-Only DATA. This is where `"helloworld!\n"` lives. The string is 12 characters + null terminator = 13 bytes. The section is 16 bytes due to alignment padding.

This is why you cannot do `char *s = "helloworld!\n"; s[0] = 'X';` — the string is in a `r--` memory region. The OS will kill your program with SIGSEGV if you try to write here.

---

### Section 14: `.eh_frame_hdr`
```
Size 0x24  VMA 0x2010  READONLY DATA
```
36 bytes. A compact index/lookup table for the `.eh_frame` data below. When a debugger or exception handler needs to unwind the stack, it uses this table to quickly find the right entry in `.eh_frame` without scanning the whole section. Think of it as a table of contents for crash handling.

---

### Section 15: `.eh_frame`
```
Size 0x7c  VMA 0x2038  READONLY DATA
```
124 bytes. Contains CFI — **Call Frame Information**. This is a series of records describing how to unwind the call stack at any given point in the code. For each function, it records: "when you're at instruction X, the return address is at stack position Y, and the saved registers are at positions Z."

Used by:
- `gdb` when printing a backtrace
- `perf` when profiling
- C++ runtime for exception propagation
- Address Sanitizer and other tools

Even though plain C has no exceptions, this data is always generated. It's what lets you see a full call stack when something crashes.

---

### Section 16: `.note.gnu.property`
```
Size 0x40  VMA 0x20b8  READONLY DATA
```
64 bytes. A structured note describing CPU and OS feature requirements. On x86-64 this typically advertises whether the binary supports Intel CET (Control-flow Enforcement Technology) — a hardware mechanism that uses a shadow stack to detect return-oriented programming (ROP) attacks.

---

### Section 17: `.note.ABI-tag`
```
Size 0x20  VMA 0x20f8  READONLY DATA
```
32 bytes. Contains the minimum kernel version this binary requires. If you copy this binary to a machine with an older kernel, the OS reads this note and refuses to run the program with a clear error, rather than crashing mysteriously later.

---

### Section 18: `.init_array`
```
Size 0x8  VMA 0x3dd0  DATA (writable)
```
8 bytes = 1 pointer (64-bit). An array of function pointers to call before `main`. The C runtime iterates this array and calls each function. You have one entry: `frame_dummy`, injected by gcc to register the exception frame data. You can add your own constructors with `__attribute__((constructor))`.

---

### Section 19: `.fini_array`
```
Size 0x8  VMA 0x3dd8  DATA (writable)
```
8 bytes = 1 pointer. Mirror of `.init_array` — functions called after `main`. One entry: `__do_global_dtors_aux`, which cleans up any C++ global destructors (none here, but the slot exists).

---

### Section 20: `.dynamic`
```
Size 0x1e0  VMA 0x3de0  DATA (writable)
```
480 bytes. The full dynamic section — all the `NEEDED`, `INIT`, `SYMTAB`, etc. entries explained above. It's writable because the dynamic linker writes into it at startup (specifically the `DEBUG` entry). After RELRO is applied, it becomes read-only.

---

### Section 21: `.got`
```
Size 0x28  VMA 0x3fc0  DATA (writable)
```
40 bytes. The **Global Offset Table** — a table of pointers to global data symbols (not functions). In a PIE binary, since nothing can use absolute addresses, every access to a global variable goes through the GOT. The dynamic linker fills in the real addresses at load time. After RELRO, this becomes read-only.

---

### Section 22: `.got.plt`
```
Size 0x20  VMA 0x3fe8  DATA (writable)
```
32 bytes. The PLT portion of the GOT — specifically holds the resolved addresses of external functions. Initially, each entry points back into the PLT resolver. After the first call to `puts`, this entry gets overwritten with the real `puts` address. This is the key mechanism of lazy binding.

This section is NOT covered by RELRO in its entirety — the entries need to stay writable for lazy binding to work. You can force all symbols to resolve at startup (`LD_BIND_NOW=1`) and then RELRO can protect this too — that's called "full RELRO."

---

### Section 23: `.data`
```
Size 0x10  VMA 0x4008  DATA (writable)
```
16 bytes. Initialized global and static variables. In this program there are none you wrote — the content here is glibc bookkeeping data (`__data_start`, `__dso_handle`). If you had written `int x = 42;` at global scope, it would live here.

---

### Section 24: `.bss`
```
Size 0x8  VMA 0x4018  ALLOC (no CONTENTS, no LOAD)
```
8 bytes. Uninitialized (or zero-initialized) global variables. Notice it has **ALLOC but not LOAD and not CONTENTS** — this section does not exist in the file on disk at all. It's pure memory reservation. At runtime, the OS allocates this memory and fills it with zeros. The one entry here (`completed.0`) is a glibc internal flag byte.

If you wrote `int y;` at global scope, it would be here — 0 bytes in the file, 4 bytes of zeroed memory at runtime.

This is why uninitialized globals are zero in C. It's not magic — `.bss` is explicitly zeroed by the OS loader before `main` runs.

---

### Section 25: `.comment`
```
Size 0x1b  VMA 0x0  READONLY (no ALLOC, no LOAD)
```
27 bytes. Not loaded into memory at all (no ALLOC). Contains a null-terminated string identifying the compiler version that built this binary, something like `GCC: (GNU) 14.2.1 20250207`. Purely informational. `strings hello_world | grep GCC` will show you this.

---

## SYMBOL TABLE

The symbol table maps human-readable names to addresses. It's what lets gdb say "you crashed in `main` at line 4" instead of "you crashed at `0x1148`." Each entry has:

```
ADDRESS  scope  type  SECTION  size  NAME
```

- **scope**: `l` = local (only visible in this file), `g` = global (visible to linker), `w` = weak (overridable)
- **type**: `F` = function, `O` = object/variable, `df` = debug file marker
- **SECTION**: which section it belongs to, or `*ABS*` (absolute, not in any section) or `*UND*` (undefined — lives in an external library)

---

### The CRT symbols (injected, not yours)

```
0x0  l  df  *ABS*  Scrt1.o
0x0  l  df  *ABS*  crtbeginS.o
0x0  l  df  *ABS*  crtendS.o
```
Debug file markers. `Scrt1.o` is the C runtime startup object for shared/PIE executables. `crtbeginS.o` and `crtendS.o` are gcc's C runtime bookend objects. They're the source of all the `_start`, `.init_array`, and exception frame machinery. You never see their source code, but they're always there.

```
0x1040  g  F  .text  _start
```
The real entry point. Injected by `Scrt1.o`. Sets up the stack frame, loads `argc`, `argv`, `envp`, calls `__libc_start_main` with a pointer to your `main`, which then calls `main`. You'll never write this, but it runs before every C program.

```
0x1070  l  F  .text  deregister_tm_clones
0x10a0  l  F  .text  register_tm_clones
```
Transaction memory support functions — register and deregister clone tables for GCC's transactional memory extension. Almost certainly unused in your program. The compiler includes them by default anyway.

```
0x10e0  l  F  .text  __do_global_dtors_aux
```
Calls global destructors (C++ `atexit` stuff). Runs after `main` returns. Walks the `.fini_array` and calls each entry.

```
0x1130  l  F  .text  frame_dummy
```
Registers this binary's exception frame data with the runtime. Called from `.init_array` before `main`. Required for proper stack unwinding.

---

### Your symbol

```
0x1139  g  F  .text  0x1a  main
```
There you are. Your `main` function. Lives at address `0x1139` in `.text`. Is `0x1a` = 26 bytes long. Global (`g`) so the linker can find it. This is the only symbol in this binary that you wrote.

---

### The external symbols (`*UND*`)

```
0x0  F  *UND*  __libc_start_main@GLIBC_2.34
0x0  F  *UND*  puts@GLIBC_2.2.5
0x0  F  *UND*  __cxa_finalize@GLIBC_2.2.5
```

`*UND*` = undefined. These functions are not in your binary. They live in `libc.so.6`. The dynamic linker resolves them at runtime by looking them up in libc's symbol table.

- **`__libc_start_main`** — called by `_start`, calls your `main`
- **`puts`** — what GCC compiled your `printf("helloworld!\n")` into. Since there are no format specifiers, `printf` with a plain string is identical to `puts` but faster (no format parsing). GCC makes this optimization automatically.
- **`__cxa_finalize`** — runs `atexit` handlers and global destructors. The `cxa` prefix is C++ ABI. It's used for C cleanup too.

---

### The weak symbols (`w`)

```
0x0  w  *UND*  _ITM_deregisterTMCloneTable
0x0  w  *UND*  _ITM_registerTMCloneTable
0x0  w  *UND*  __gmon_start__
```

Weak symbols (`w`) are optional. If the dynamic linker finds them in a loaded library, great — it uses them. If not, it sets them to null and the program continues. These three are:

- **`_ITM_*`** — Intel Transactional Memory support hooks. Not present on most systems. Safely ignored.
- **`__gmon_start__`** — gprof profiling hook. If you compiled with `-pg`, this would be defined. Without `-pg`, it stays null and nothing happens.

---

### The data/BSS boundary symbols

```
0x4008  w   .data  data_start
0x4008  g   .data  __data_start
0x4018  g   .bss   __bss_start
0x4018  g   .data  _edata
0x4020  g   .bss   _end
```

These are sentinel symbols marking the boundaries between sections. They don't point to variables — they point to locations. Useful for tools and the runtime:

- `data_start` / `__data_start` — beginning of `.data` at `0x4008`
- `__bss_start` / `_edata` — end of `.data` / start of `.bss` at `0x4018`
- `_end` — end of `.bss` at `0x4020`. This is the last byte of the entire program's statically allocated memory.

The C library's `malloc` historically used `_end` as the starting point for the heap (memory allocated with `brk()`). On modern systems ASLR moves things around, but these symbols remain for compatibility.

---

### The hidden symbols

```
0x1154  g  F  .fini   .hidden  _fini
0x4010  g  O  .data   .hidden  __dso_handle
0x4018  g  O  .data   .hidden  __TMC_END__
```

`.hidden` means these symbols are visible within this binary but not exported to other shared libraries. They're global within the process but private to this binary:

- **`_fini`** — the destructor entry point, runs after main
- **`__dso_handle`** — DSO = Dynamic Shared Object. A pointer to this binary's own handle, used by `__cxa_finalize` to know which binary's destructors to run
- **`__TMC_END__`** — end of the Thread-local storage / transactional memory area

---

## The full picture

Here is how everything connects when you type `./hello_world`:

1. **Kernel** reads the ELF header, finds `INTERP`, loads the dynamic linker from `/lib64/ld-linux-x86-64.so.2`
2. **Dynamic linker** reads `DYNAMIC`, sees `NEEDED libc.so.6`, loads libc into memory
3. **Dynamic linker** reads `.rela.plt`, finds the `puts` entry in `.dynsym`, looks up `puts` in libc's symbol table, writes the real `puts` address into `.got.plt`
4. **Dynamic linker** checks Version References, verifies libc is `>= GLIBC_2.34`
5. **Dynamic linker** calls `.init` and walks `.init_array`, calling `frame_dummy`
6. **Dynamic linker** calls `__libc_start_main` with a pointer to your `main`
7. **`__libc_start_main`** sets up `argc`/`argv`, calls your `main` at `0x1139`
8. **`main`** runs — 26 bytes of machine code — calls `puts` via `.plt` which jumps through `.got.plt` to the real `puts` in libc
9. **`puts`** writes `helloworld!\n` to stdout by calling the kernel's `write` syscall
10. **`main`** returns `0` to `__libc_start_main`
11. **`__libc_start_main`** calls `__cxa_finalize`, walks `.fini_array`, calls `_fini`, then calls the `exit` syscall
12. **Kernel** receives `exit(0)`, reclaims all memory, the shell sees `$?` = 0

All of that. For `printf("helloworld!\n")`.

>[!IMPORTANT]
> This is all for a simple objdump headers file explanation. It does not include any disassembly language stuff. Though I am not a perfect disassembly decoder, I am currently using AI to learn it. This is an attribution to the fact that every piece of code is written by me and not by any AI slop stuff. I rely on AI only for better understanding and explanations of concept and to check whether my assumptions and questions are corelated or not. Till now, these are parallel. For disassembly, I would have to create a hello_world.disassembly.md, explanation follows. Nice to get into those stuffs. Not so deep, I am reading these stuffs, learning them actively, but not so perfect. I will come back to this point, after 3 months to check if I could understand these better than now. All these files are actively reserved for my future versions. Let's see how C progresses.
