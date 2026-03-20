Let's use your actual `helloworld.c` as the example for everything.

---

## NvimTree — `Ctrl+m`
Open nvim, press `Ctrl+m`. A sidebar appears showing your Phase1 folder. Navigate with `j/k`, press `Enter` to open `helloworld.c`. Press `Ctrl+m` again to close it.

---

## Telescope — `Ctrl+f` and `Ctrl+g`

You're inside nvim and want to open `helloworld.another.c` without typing the path:
```
Ctrl+f  → type "another" → press Enter → file opens
```

You remember writing `escape characters` somewhere but forgot which file:
```
Ctrl+g  → type "escape" → see every file containing that word → Enter to jump there
```

---

## LSP — `gd`, `gr`, `K`, `F2`

You're in `helloworld.c`, cursor is on `printf`:

```
K       → popup shows: "printf(const char *format, ...) — print formatted output"
gd      → jumps into stdio.h where printf is declared
gr      → shows every line in your project that calls printf
```

You have a function called `greet` across 5 files and want to rename it to `say_hello`:
```
cursor on "greet" → F2 → type "say_hello" → Enter → renamed everywhere instantly
```

---

## Insert Mode Saves — `Ctrl+s`

You're typing code, mid-sentence in insert mode, want to save without reaching for Escape:
```
Ctrl+s  → saves the file → cursor stays, keep typing
```

Done making changes for the day:
```
Ctrl+Shift+s  → saves and closes nvim in one shot
```

---

The flow for your daily C work will look like: `Ctrl+m` to find a file → open it → `K` to check what functions do → `gd` to explore them → `Ctrl+s` to save. That's 80% of your editor usage right there.

For more information, see the mappings in lua options.
