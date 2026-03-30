### The First GDB Analysis.

This is my first time, doing a gdb analysis of my insertion sort code; what is it? It will be in the Practice/C-Arrays/Basics;... stuff; find it.
In order to get the gdb output a better, a good output; I need to first tell my gcc compiler that : Hey, I am going to debug this binary, so insert all the debugging options for this binary; How do I tell it?

```bash
gcc -g -o <binary> <mycfile.c>
# This -g flag tells gcc to embed debug info into the binary. Without it, the gdb is literally blind.
```

Next is the most simple step of running all command line tools : running it;

```bash
gdb ./<binary>
```

Now that, we can cat it, redirect it, pipe its ouput, do whatever we want to to do with it.. : but you might need to copy it; because the gdb drops you in an interactive debugging environment.

To start with, I am not going to claude or codex gpt; the interactive environment itself says : help; type help to get the help; If you do not know what to do? main gdb; deal over; And that is what I am going to do...
