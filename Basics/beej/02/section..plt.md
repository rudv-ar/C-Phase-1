# What is a .plt section? what does it contain?

As per the disassembly output.. for a simple optimised hello world program : the .plt section is exactly this :

```
Disassembly of section .plt:

0000000000001020 <puts@plt-0x10>: 
1020:	ff 35 ca 2f 00 00    	push   0x2fca(%rip)        # 3ff0 <_GLOBAL_OFFSET_TABLE_+0x8>
    1026:	ff 25 cc 2f 00 00    	jmp    *0x2fcc(%rip)        # 3ff8 <_GLOBAL_OFFSET_TABLE_+0x10>
    102c:	0f 1f 40 00          	nopl   0x0(%rax)

0000000000001030 <puts@plt>:
    1030:	ff 25 ca 2f 00 00    	jmp    *0x2fca(%rip)        # 4000 <puts@GLIBC_2.2.5>
    1036:	68 00 00 00 00       	push   $0x0
    103b:	e9 e0 ff ff ff       	jmp    1020 <_init+0x20>

```



## 1. The Problem It Solves

When you write `puts("hello")`, that function lives in `libc.so` — a shared library that is:

- Loaded at **runtime**, not compile time
- Mapped to a **different virtual address** every run (ASLR)
- **Shared** across every process on the system

Your binary is compiled hours or days before it runs. It has no idea where `puts` will land in memory. So how does `call puts` work at all?

The answer is a two-part indirection system: the **PLT** and the **GOT**.

---

## 2. The Two Tables

| Section | Full Name | Type | Purpose |
|---|---|---|---|
| `.plt` | Procedure Linkage Table | Code (read+exec) | Trampolines — small stubs that jump via the GOT |
| `.got` / `.got.plt` | Global Offset Table | Data (read+write) | Array of addresses filled in by the dynamic linker |

Your binary knows the address of its own `.plt` at compile time — it's in the binary itself. The `.plt` then jumps *through* the `.got`, which the dynamic linker patches at runtime with the real addresses.

```
Your binary (known at compile time)      libc (unknown until runtime)
─────────────────────────────────        ──────────────────────────────
  .text:  call puts@plt          ──→  .plt stub  ──→  .got entry  ──→  puts()
```

---

## 3. Anatomy of the `.plt` in Your Binary

```asm
Disassembly of section .plt:

; ── PLT[0]: The resolver stub ──────────────────────────────────────────────
0000000000001020 <puts@plt-0x10>:
    1020: ff 35 ca 2f 00 00   push 0x2fca(%rip)   # push GOT[1] (link_map ptr)
    1026: ff 25 cc 2f 00 00   jmp  *0x2fcc(%rip)  # jump to GOT[2] (_dl_runtime_resolve)
    102c: 0f 1f 40 00         nopl 0x0(%rax)       # alignment padding

; ── PLT[1]: puts stub ───────────────────────────────────────────────────────
0000000000001030 <puts@plt>:
    1030: ff 25 ca 2f 00 00   jmp  *0x2fca(%rip)  # jump to GOT[puts]
    1036: 68 00 00 00 00      push $0x0            # puts' relocation index
    103b: e9 e0 ff ff ff      jmp  1020            # jump to resolver stub
```

### Line by Line

**`1030: jmp *GOT[puts]`**
- Indirect jump — reads an address *stored in* the GOT, jumps there
- On first call: GOT[puts] points back to `1036` (not yet resolved)
- After first call: GOT[puts] contains the real address of `puts` in libc

**`1036: push $0x0`**
- Only reached on first call (when GOT hasn't been patched yet)
- Pushes the relocation index — tells the resolver *which* symbol to look up
- `$0x0` = index 0 = `puts` (first imported symbol)

**`103b: jmp 1020`**
- Jumps to PLT[0], the universal resolver stub

**`1020: push GOT[1]`**
- Pushes a pointer to the `link_map` struct (the loaded library list)
- The resolver needs this to know which binary triggered the call

**`1026: jmp GOT[2]`**
- Jumps into `_dl_runtime_resolve` inside `ld.so` (the dynamic linker)
- This is where the actual symbol lookup happens

---

## 4. Lazy Binding — Step by Step

This mechanism is called **lazy binding**: symbols are resolved only on first use, not at program startup.

### First Call to `puts`

```
Your code                    PLT                    GOT               ld.so / libc
─────────────────────────────────────────────────────────────────────────────────
call puts@plt    ──→    jmp *GOT[puts]
                              │
                              ↓ GOT[puts] not yet patched — points back to PLT+6
                         push $0x0  (relocation index)
                         jmp PLT[0]
                              │
                              ↓
                         push GOT[1]  (link_map)
                         jmp GOT[2]   ──→   _dl_runtime_resolve()
                                                 │
                                                 ↓
                                          lookup "puts" in libc
                                          find address: 0x7f4a_b3c2_1080
                                          write it into GOT[puts]  ← patch!
                                          jump to puts()  ──→  puts() runs ✓
```

### Every Subsequent Call

```
Your code                    PLT                    GOT               libc
──────────────────────────────────────────────────────────────────────────
call puts@plt    ──→    jmp *GOT[puts]
                              │
                              ↓ GOT[puts] NOW contains 0x7f4a_b3c2_1080
                              └──────────────────────────────────→ puts() runs ✓
```

The resolver is **never invoked again**. One lookup, cached forever.

---

## 5. Memory Layout During Execution

```
Virtual Address Space (simplified)
┌──────────────────────────────────────┐  0x0000000000001000
│  .text  (your code)                  │
│    call puts@plt  →  0x1030          │
├──────────────────────────────────────┤  0x0000000000001020
│  .plt                                │
│    PLT[0]: resolver stub             │
│    PLT[1]: puts stub (jmp *GOT[puts])│
├──────────────────────────────────────┤  0x0000000000003fb8
│  .got.plt                            │
│    GOT[0]: address of .dynamic       │
│    GOT[1]: link_map ptr  (ld fills)  │
│    GOT[2]: _dl_runtime_resolve (ld)  │
│    GOT[3]: puts real addr (lazily)   │  ← starts as &PLT[1]+6, patched on call
├──────────────────────────────────────┤  0x00007f....
│  libc.so  (mapped by ld.so)          │
│    puts() at some ASLR address       │
└──────────────────────────────────────┘
```

---

## 6. Why Not Resolve Everything at Startup?

You could — this is called **eager binding** (`LD_BIND_NOW=1` or `-z now` linker flag).

| | Lazy Binding (default) | Eager Binding (`-z now`) |
|---|---|---|
| Startup time | Fast — resolve only what's used | Slower — resolve everything upfront |
| Unused symbols | Never resolved | Resolved even if never called |
| GOT at runtime | Writable (patchable) | Can be made read-only (RELRO) |
| Security | GOT is attackable | GOT can be locked down |

Lazy binding was the historical default for performance. Modern security-hardened binaries use eager binding + Full RELRO to make the GOT read-only.

---

## 7. Security Implications

### 7.1 GOT Overwrite Attack

The GOT is writable by default. If an attacker can write an arbitrary 8-byte value anywhere in memory (e.g. via a heap overflow or format string `%n`), they target `GOT[puts]` and replace the real address with their shellcode or ROP gadget address.

```
Normal:   call puts@plt → jmp *GOT[puts] → 0x7f...puts()  ✓
Attacked: call puts@plt → jmp *GOT[puts] → 0x7f...system() ← attacker wrote this
```

Every subsequent `puts()` call now executes `system()` instead. Classic.

### 7.2 RELRO — The Defense

**RELRO** (Relocation Read-Only) is a linker/loader mitigation:

| Level | What It Does |
|---|---|
| Partial RELRO | Reorders sections, makes `.got` (not `.got.plt`) read-only |
| Full RELRO | Forces eager binding, then marks entire GOT read-only after startup |

Full RELRO completely defeats GOT overwrites — the GOT is read-only by the time your `main()` runs.

### 7.3 Checking Your Binary

```bash
# Install checksec
checksec --file=putsv2

# Or read the ELF headers manually
readelf -l putsv2 | grep GNU_RELRO
```

Output to look for:

```
RELRO:    Full RELRO      ← GOT is read-only, GOT overwrite impossible
RELRO:    Partial RELRO   ← .got.plt still writable, GOT overwrite possible
RELRO:    No RELRO        ← everything writable, fully vulnerable
```

### 7.4 Other PLT/GOT Attack Techniques

| Technique | What Happens |
|---|---|
| GOT overwrite | Patch GOT entry to redirect function calls |
| PLT trampoline abuse | Use PLT stubs as ROP gadgets (they contain useful `jmp` patterns) |
| GOT leak | Read GOT entry to get libc base address → defeat ASLR |
| Ret2plt | Return into a PLT stub to call libc functions without shellcode |

The **GOT leak** is particularly important: if you can read `GOT[puts]`, you know the exact runtime address of `puts` in libc. Since all of libc is loaded contiguously, you can compute the address of *any* libc function — `system`, `execve`, etc. — by adding a known offset. This defeats ASLR entirely.

---

## 8. Forcing Eager Binding (Full RELRO)

```bash
# Compile with full RELRO + no-lazy binding
clang -O2 -Wl,-z,relro,-z,now -o putsv2_hardened putsv1.c

# Verify
checksec --file=putsv2_hardened
```

With Full RELRO, the `.plt` stubs still exist (the code doesn't change), but after `ld.so` resolves all symbols at startup and patches the GOT, it calls `mprotect()` to flip the GOT pages to read-only. Any write attempt causes a segfault.

---

## 9. TL;DR

```
.plt  = a set of small jump stubs, one per imported function
.got  = a table of addresses, filled in by the dynamic linker

First call:  PLT stub → GOT (not patched) → resolver → libc → patch GOT → run function
Later calls: PLT stub → GOT (patched)     → libc function directly

Security risk: GOT is writable → attacker can redirect any libc call
Defense:       Full RELRO makes GOT read-only after startup
```

The `.plt` exists because shared libraries load at unpredictable addresses (ASLR). It is the price of dynamic linking — and the attack surface that comes with it.

