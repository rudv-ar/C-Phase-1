```
════════════════════════════════════════════════════════════════════
        COMPLETE LINUX x86_64 VIRTUAL MEMORY LAYOUT
        High Address → Low Address
════════════════════════════════════════════════════════════════════

0xFFFFFFFFFFFFFFFF ┌─────────────────────────────────────────────┐
                   │         KERNEL SPACE                        │
                   │  (process cannot touch this)                │
                   │  syscall handlers, scheduler, drivers       │
                   │  accessible only via syscall instruction    │
0xFFFF800000000000 └─────────────────────────────────────────────┘
                   │  CANONICAL HOLE  (unmapped, hardware limit) │
                   │  x86_64 only uses 48-bit virtual addresses  │
                   │  anything between 0x7FFF... and 0xFFFF8...  │
                   │  is unmapped — accessing it = SIGSEGV       │
0x00007FFFFFFFFFFF ┌─────────────────────────────────────────────┐
                   │                                             │
                   │         STACK                               │
                   │         grows DOWNWARD ↓                    │
                   │                                             │
                   │  ┌───────────────────────────────────┐      │
                   │  │ env vars, argv, argc              │      │
                   │  ├───────────────────────────────────┤      │
                   │  │         main() frame              │      │
                   │  │  ┌─────────────────────────────┐  │      │
                   │  │  │ return addr → __libc_start  │  │      │
                   │  │  │ saved RBP                   │  │      │
                   │  │  │ int size = 11               │  │      │
                   │  │  │ array[10] = 33              │  │      │
                   │  │  │ array[9]  = 44              │  │      │
                   │  │  │ array[8]  = -21             │  │      │
                   │  │  │ array[7]  = 90              │  │      │
                   │  │  │ array[6]  = 10              │  │      │
                   │  │  │ array[5]  = 11              │  │      │
                   │  │  │ array[4]  = 34              │  │      │
                   │  │  │ array[3]  = 21              │  │      │
                   │  │  │ array[2]  = 7               │  │      │
                   │  │  │ array[1]  = 5               │  │      │
                   │  │  │ array[0]  = 1  ←0x7fffffe710│  │      │
                   │  │  └── RSP points here ──────────┘  │      │
                   │  │         insertionSort() frame     │      │
                   │  │  ┌─────────────────────────────┐  │      │
                   │  │  │ return addr → main+offset   │  │      │
                   │  │  │ saved RBP                   │  │      │
                   │  │  │ int key                     │  │      │
                   │  │  │ int l                       │  │      │
                   │  │  │ int i  (loop counter)       │  │      │
                   │  │  │ int *arr  (pointer to array)│  │      │
                   │  │  │ int size  (copy of 11)      │  │      │
                   │  │  └─────────────────────────────┘  │      │
                   │  │         printArray() frame        │      │
                   │  │  ┌─────────────────────────────┐  │      │
                   │  │  │ return addr → main+offset   │  │      │
                   │  │  │ saved RBP                   │  │      │
                   │  │  │ int i  (loop counter)       │  │      │
                   │  │  │ int *arr                    │  │      │
                   │  │  │ int size                    │  │      │
                   │  │  └─────────────────────────────┘  │      │
                   │  └───────────────────────────────────┘      │
                   │  RED ZONE (128 bytes below RSP)             │
                   │  ABI guaranteed safe, no sub rsp needed     │
                   │                                             │
0x00007FF000000000 └─────────────────────────────────────────────┘
                   │                                             │
                   │  ASLR GAP  (randomly sized unmapped region) │
                   │  kernel randomizes this at every exec       │
                   │  makes stack address unpredictable          │
                   │  defeats naive ROP/buffer overflow attacks  │
                   │                                             │
                   ┌─────────────────────────────────────────────┐
                   │         MMAP REGION                         │
                   │  shared libraries loaded here               │
                   │  libc.so   → your printf, puts, malloc      │
                   │  libm.so   → math functions                 │
                   │  ld.so     → dynamic linker itself          │
                   │  grows DOWNWARD ↓                           │
                   └─────────────────────────────────────────────┘
                   │                                             │
                   │  ASLR GAP  (unmapped between mmap & heap)   │
                   │                                             │
                   ┌─────────────────────────────────────────────┐
                   │         HEAP                                │
                   │         grows UPWARD ↑                      │
                   │                                             │
                   │  malloc(24) → carves chunk from here        │
                   │  addresses look like 0x555555559...         │
                   │  managed by glibc's ptmalloc2               │
                   │  free() returns chunk back to allocator     │
                   │  brk() / mmap() syscalls grow the heap      │
                   │                                             │
                   └─────────────────────────────────────────────┘
                   ┌─────────────────────────────────────────────┐
                   │         BSS  segment                        │
                   │  uninitialized global & static variables    │
                   │  int x;  (global, no value)                 │
                   │  zeroed out by OS at program start          │
                   │  takes no space in the binary on disk       │
                   └─────────────────────────────────────────────┘
                   ┌─────────────────────────────────────────────┐
                   │         DATA  segment                       │
                   │  initialized global & static variables      │
                   │  int x = 5;  (global, has value)            │
                   │  static int y = 99;                         │
                   │  lives in binary on disk, loaded at start   │
                   └─────────────────────────────────────────────┘
                   ┌─────────────────────────────────────────────┐
                   │         RODATA  segment  (.rodata)          │
                   │  read-only data                             │
                   │  your string literals live here             │
                   │  "Array Before Sorting : "  ← HERE          │
                   │  "A value of..."            ← HERE          │
                   │  const char *name = "Orio"  ← HERE          │
                   │  write to this = SIGSEGV                    │
                   └─────────────────────────────────────────────┘
                   ┌─────────────────────────────────────────────┐
                   │         TEXT  segment  (.text)              │
                   │  your actual compiled machine code          │
                   │  insertionSort lives here as opcodes        │
                   │  printArray lives here as opcodes           │
                   │  main lives here as opcodes                 │
                   │  read + execute, never write                │
                   │  address looks like 0x555555555...          │
                   └─────────────────────────────────────────────┘
                   ┌─────────────────────────────────────────────┐
                   │  NULL / UNMAPPED REGION                     │
                   │  page 0 intentionally unmapped              │
                   │  NULL pointer dereference = SIGSEGV         │
                   │  not an accident — done on purpose          │
0x0000000000000000 └─────────────────────────────────────────────┘

════════════════════════════════════════════════════════════════════
        STACK FRAME INTERNALS — what "return address" means
════════════════════════════════════════════════════════════════════

When main calls insertionSort:

  1. CPU pushes return address onto stack
     → the address of the instruction in main AFTER the call
     → so when insertionSort hits `ret`, CPU jumps back there

  2. insertionSort pushes saved RBP
     → saves main's frame base so it can be restored

  3. insertionSort sets RBP = RSP
     → now RBP is the base of insertionSort's frame

  4. insertionSort moves RSP down
     → sub rsp, N  makes room for key, l, i

  visually:

          main's RBP  → [ saved RBP of caller  ]
                        [ return addr to libc  ]
                        [ int size             ]
                        [ array[0..10]         ]  ← your data
  insertionSort's RBP → [ saved RBP of main    ]
                        [ return addr to main  ]  ← address inside main
                        [ int *arr             ]
                        [ int size             ]
                        [ int i                ]
                        [ int l                ]
  RSP         →         [ int key              ]  ← top of stack

════════════════════════════════════════════════════════════════════
        KEY REGISTERS
════════════════════════════════════════════════════════════════════

  RIP  — instruction pointer. always points to NEXT instruction to execute
  RSP  — stack pointer.       always points to TOP of stack (lowest address)
  RBP  — base pointer.        points to bottom of CURRENT frame

════════════════════════════════════════════════════════════════════
        WHAT ASLR DOES TO YOUR ADDRESSES
════════════════════════════════════════════════════════════════════

  without ASLR:  array always at 0x7fffffffe710  (predictable, exploitable)
  with    ASLR:  array at different address every run
                 but TEXT segment (your code) stays at 0x555555555...
                 on Linux with PIE disabled

  you can see ASLR in action:
  run your program twice, print &array[0] both times — different each time

════════════════════════════════════════════════════════════════════
        WHAT YOU SAW IN GDB
════════════════════════════════════════════════════════════════════

  0x7fffffffe710  → array[0]         STACK
  0x555555555245  → insertionSort    TEXT  (your code as opcodes)
  0x555555559...  → malloc chunks    HEAP  (you'll see this soon)
  "Array Before"  → 0x555555556...  RODATA (your string literal)
```

That's the complete picture. Everything your process owns, from `0x0` to `0xFFFF...`, exactly as Linux sees it.
