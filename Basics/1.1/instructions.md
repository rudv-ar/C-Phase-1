# How to run the C program?

In linux, gnu system, there exists the gcc - The GNU C Compiler, using which we will be able to build the C program which has the extension as  file.c 

## gcc --help

Usage: gcc [options] file...
Options:
  -pass-exit-codes         Exit with highest error code from a phase.
  --help                   Display this information.
  --target-help            Display target specific command line options (including assembler and linker options).
  --help={common|optimizers|params|target|warnings|[^]{joined|separate|undocumented}}[,...].
                           Display specific types of command line options.
  (Use '-v --help' to display command line options of sub-processes).
  --version                Display compiler version information.
  -dumpspecs               Display all of the built in spec strings.
  -dumpversion             Display the version of the compiler.
  -dumpmachine             Display the compiler's target processor.
  -foffload=<targets>      Specify offloading targets.
  -print-search-dirs       Display the directories in the compiler's search path.
  -print-libgcc-file-name  Display the name of the compiler's companion library.
  -print-file-name=<lib>   Display the full path to library <lib>.
  -print-prog-name=<prog>  Display the full path to compiler component <prog>.
  -print-multiarch         Display the target's normalized GNU triplet, used as
                           a component in the library path.
  -print-multi-directory   Display the root directory for versions of libgcc.
  -print-multi-lib         Display the mapping between command line options and
                           multiple library search directories.
  -print-multi-os-directory Display the relative path to OS libraries.
  -print-sysroot           Display the target libraries directory.
  -print-sysroot-headers-suffix Display the sysroot suffix used to find headers.
  -Wa,<options>            Pass comma-separated <options> on to the assembler.
  -Wp,<options>            Pass comma-separated <options> on to the preprocessor.
  -Wl,<options>            Pass comma-separated <options> on to the linker.
  -Xassembler <arg>        Pass <arg> on to the assembler.
  -Xpreprocessor <arg>     Pass <arg> on to the preprocessor.
  -Xlinker <arg>           Pass <arg> on to the linker.
  -save-temps              Do not delete intermediate files.
  -save-temps=<arg>        Do not delete intermediate files.
  -no-canonical-prefixes   Do not canonicalize paths when building relative
                           prefixes to other gcc components.
  -pipe                    Use pipes rather than intermediate files.
  -time                    Time the execution of each subprocess.
  -specs=<file>            Override built-in specs with the contents of <file>.
  -std=<standard>          Assume that the input sources are for <standard>.
  --sysroot=<directory>    Use <directory> as the root directory for headers
                           and libraries.
  -B <directory>           Add <directory> to the compiler's search paths.
  -v                       Display the programs invoked by the compiler.
  -###                     Like -v but options quoted and commands not executed.
  -E                       Preprocess only; do not compile, assemble or link.
  -S                       Compile only; do not assemble or link.
  -c                       Compile and assemble, but do not link.
  -o <file>                Place the output into <file>.
  -pie                     Create a dynamically linked position independent
                           executable.
  -shared                  Create a shared library.
  -x <language>            Specify the language of the following input files.
                           Permissible languages include: c c++ assembler none
                           'none' means revert to the default behavior of
                           guessing the language based on the file's extension.

Options starting with -g, -f, -m, -O, -W, or --param are automatically
 passed on to the various sub-processes invoked by gcc.  In order to pass
 other options on to these processes the -W<letter> options must be used.

For bug reporting instructions, please see:
<https://gitlab.archlinux.org/archlinux/packaging/packages/gcc/-/issues>

## Note that the file name must be provided only at the last

gcc -Wall -Wextra -Wpedantic -std=c11 -o helloworldv2 helloworld.c

What does this flag do?

Breaking it down piece by piece:

```
gcc -Wall -Wextra -Wpedantic -std=c11 -o helloworldv2 helloworld.c
```

**`gcc`** — invoke the GNU C Compiler.

**`-Wall`** — enable "all" common warnings. Catches things like unused variables, missing return types, suspicious code. The name is a bit misleading — it's not *literally* all warnings, just the most useful ones.

**`-Wextra`** — enable *extra* warnings on top of `-Wall`. Catches subtler issues like unused function parameters, comparisons that might not behave as expected.

**`-Wpedantic`** — strictly enforce the C standard. Warns about anything that's technically allowed by your compiler (gcc) but isn't in the official C spec. This is where `int main()` vs `int main(void)` gets flagged.

**`-std=c11`** — compile using the C11 standard (published 2011). This tells gcc exactly which version of C you're writing. You could also use `c99`, `c17`, `gnu11`, etc.

**`-o helloworldv2`** — name the output binary `helloworldv2`. Without `-o`, gcc defaults to a file called `a.out` every time, which gets overwritten on each compile.

**`helloworld.c`** — the input source file to compile.


So the full sentence in plain English: *"Compile `helloworld.c` using the C11 standard, with all warnings enabled, strictly following the spec, and save the result as `helloworldv2`."*

The `-W` flags don't change how your program runs — they just make gcc more talkative about potential problems while building. Think of them as turning up the compiler's critical eye. You always want them on while learning.
