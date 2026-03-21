#!/bin/bash

# =============================================================================
# install.c.sh — C Development Environment Installer
# Target : Arch Linux (pacman-based systems)
# Author : rudv-ar
# Ref    : https://rudv-ar.github.io/pages/Cpath.html#install
# =============================================================================
#
# This script sets up a complete, professional C development environment from
# scratch. It installs the compiler toolchain, debuggers, a terminal-based
# editor (Neovim) with real-time LSP support via clangd, and writes the
# necessary config files automatically.
#
# The installer is interactive — it asks before doing anything destructive,
# explains what each step does, and gives you a choice of editor framework
# (NvChad or LazyVim). Nothing is silent and nothing is assumed.
#
# Usage:
#   chmod +x install.c.sh
#   ./install.c.sh
#
# Flags:
#   --needed   Pass --needed to pacman so already-installed packages are
#              skipped rather than reinstalled. Recommended on partial setups.
#   --dry-run  Print every command that would run without executing any of
#              them. Useful for inspecting the script before committing.
# =============================================================================

set -euo pipefail
# set -e  : exit immediately if any command returns a non-zero exit code.
#           this prevents the script from silently continuing after an error.
# set -u  : treat unset variables as errors. catches typos in variable names.
# set -o pipefail : if any command in a pipe fails, the whole pipe fails.
#                   without this, 'cmd1 | cmd2' succeeds even if cmd1 fails.


# =============================================================================
# SECTION 1 — COLOUR VARIABLES
# All colour codes are lowercase, as convention dictates that uppercase names
# are reserved for exported environment variables (PATH, HOME, CC, etc.).
# =============================================================================

reset='\033[0m'       # reset all formatting — always use after a coloured string

# standard text colours (0;XX = normal weight)
black='\033[0;30m'
red='\033[0;31m'
green='\033[0;32m'
yellow='\033[0;33m'
blue='\033[0;34m'
purple='\033[0;35m'
cyan='\033[0;36m'
white='\033[0;37m'

# bold text colours (1;XX = bold weight)
bred='\033[1;31m'
bgreen='\033[1;32m'
byellow='\033[1;33m'
bcyan='\033[1;36m'
bpurple='\033[1;35m'
bwhite='\033[1;37m'


# =============================================================================
# SECTION 2 — PROMPT PREFIX TEMPLATES
# These are reusable prefix strings for consistent, structured log output.
# Every printf call in this script uses one of these prefixes so the output
# looks uniform and is easy to read at a glance.
# =============================================================================

tag="${bcyan}[INSTALL.C]${reset}"
info_tag="${bcyan}[INSTALL.C · ${cyan}info${bcyan}]${reset}"
ok_tag="${bcyan}[INSTALL.C · ${bgreen}ok${bcyan}]${reset}"
warn_tag="${bcyan}[INSTALL.C · ${byellow}warn${bcyan}]${reset}"
err_tag="${bcyan}[INSTALL.C · ${bred}error${bcyan}]${reset}"
ask_tag="${bcyan}[INSTALL.C · ${bpurple}?${bcyan}]${reset}"
step_tag="${bcyan}[INSTALL.C · ${byellow}step${bcyan}]${reset}"
pkg_tag="${bcyan}[INSTALL.C · ${bgreen}pkg${bcyan}]${reset}"
done_tag="${bcyan}[INSTALL.C · ${bgreen}done${bcyan}]${reset}"


# =============================================================================
# SECTION 3 — FLAG PARSING
# Read command-line arguments passed to the script.
# --needed  : adds --needed to every pacman call (skip already-installed pkgs)
# --dry-run : print commands instead of running them (safe inspection mode)
# =============================================================================

pacman_needed=""   # will be set to "--needed" if --needed flag is passed
dry_run=false      # will be set to true if --dry-run flag is passed

for arg in "$@"; do
    case "$arg" in
        --needed)
            pacman_needed="--needed"
            printf "${info_tag} ${yellow}--needed${reset} flag detected — pacman will skip already-installed packages.\n"
            ;;
        --dry-run)
            dry_run=true
            printf "${warn_tag} ${yellow}--dry-run${reset} mode — no commands will be executed. Printing only.\n"
            ;;
        *)
            printf "${warn_tag} Unknown flag: ${yellow}${arg}${reset} — ignoring.\n"
            ;;
    esac
done


# =============================================================================
# SECTION 4 — HELPER FUNCTIONS
# Small utility functions used throughout the script.
# =============================================================================

# run_cmd — execute a command, or print it if --dry-run is active.
# usage: run_cmd sudo pacman -S --needed neovim
run_cmd() {
    if [[ "$dry_run" == true ]]; then
        printf "${warn_tag} ${yellow}[dry-run]${reset} would run: ${white}$*${reset}\n"
    else
        "$@"
    fi
}

# section_header — print a clean accent line with a section title.
# no box drawing, no padding — just a clear visual break between steps.
# usage: section_header "Step 1 — System Upgrade"
section_header() {
    printf "\n${bcyan}══ $1 ${reset}\n\n"
}

# ask_yn — prompt the user with a yes/no question.
# sets the global variable $reply to "yes" or "no".
# usage: ask_yn "Do you want to continue?"
ask_yn() {
    printf "${ask_tag} ${bwhite}$1${reset} ${purple}(yes/no)${reset} : "
    read -r reply
    reply="${reply,,}"   # convert to lowercase so YES / Yes / yes all work
}

# check_cmd — verify a command exists on PATH after installation.
# exits with an error message if it does not.
# usage: check_cmd gcc
check_cmd() {
    if ! command -v "$1" &>/dev/null; then
        printf "${err_tag} Command not found after install: ${bred}$1${reset}\n"
        printf "${err_tag} Something went wrong. Check the pacman output above.\n"
        exit 1
    fi
    printf "${ok_tag} ${green}$1${reset} → $(command -v "$1")\n"
}

# write_file — write content to a file, respecting --dry-run.
# usage: write_file "/path/to/file" "content"
write_file() {
    local filepath="$1"
    local content="$2"
    if [[ "$dry_run" == true ]]; then
        printf "${warn_tag} ${yellow}[dry-run]${reset} would write to: ${white}${filepath}${reset}\n"
    else
        mkdir -p "$(dirname "$filepath")"
        printf '%s\n' "$content" > "$filepath"
        printf "${ok_tag} Written: ${green}${filepath}${reset}\n"
    fi
}


# =============================================================================
# SECTION 5 — PRE-FLIGHT: WELCOME SCREEN + ARCH CHECK
# Print the installer header with spaced information, then verify we are on
# an Arch-based system before attempting anything.
# =============================================================================

clear
sleep 1

printf "\n\n"
printf "${bcyan}  C Development Environment Installer${reset}\n"
printf "\n"
printf "  author    ${purple}rudv-ar${reset}\n"
printf "  target    ${green}Arch Linux${reset} · pacman\n"
printf "  ref       ${blue}https://rudv-ar.github.io/pages/Cpath.html#install${reset}\n"
printf "\n"
sleep 1

printf "${info_tag} This installer sets up a complete, professional C development\n"
printf "           environment — compiler toolchain, debuggers, Neovim with\n"
printf "           clangd LSP, and useful shell aliases. It is fully interactive\n"
printf "           and asks before changing anything on your system.\n"
printf "\n"
sleep 1

printf "${warn_tag} This script targets ${green}Arch Linux${reset} and uses ${green}pacman${reset} as the package\n"
printf "           manager. Running it on other distros will not work.\n"
printf "\n"
sleep 1

# check for pacman — the Arch package manager
if ! command -v pacman &>/dev/null; then
    printf "${err_tag} ${bred}pacman not found.${reset} Exiting — this script requires Arch Linux.\n"
    exit 1
fi

printf "${ok_tag} ${green}pacman${reset} found — confirmed Arch-based system.\n\n"
sleep 1


# =============================================================================
# SECTION 6 — PACKAGE LIST DISPLAY + CONFIRMATION
# Show the user exactly what will be installed and ask for confirmation before
# touching the system. Never install silently without consent.
# =============================================================================

section_header "Packages to Install"

printf "${pkg_tag} The following packages will be installed.\n\n"

printf "${byellow}  toolchain${reset}\n\n"

printf "  ${green}base-devel${reset}\n"
printf "    Package group: gcc, make, binutils, fakeroot, and other essential\n"
printf "    build tools. Mandatory — everything else depends on it.\n\n"

printf "  ${green}gcc${reset}\n"
printf "    GNU C Compiler. Translates .c source files into machine code through\n"
printf "    four stages: preprocessing, compilation, assembly, and linking.\n"
printf "    The same compiler used to build the Linux kernel.\n\n"

printf "  ${green}gdb${reset}\n"
printf "    GNU Debugger. Set breakpoints, step through code line by line, inspect\n"
printf "    variables and memory. When your program segfaults, gdb tells you why.\n\n"

printf "  ${green}clang${reset}\n"
printf "    LLVM C compiler. Friendlier error messages than gcc, better static\n"
printf "    analysis. Also ships clangd — the language server that powers IDE\n"
printf "    features in your editor: autocomplete, hover docs, inline diagnostics.\n\n"

printf "  ${green}lldb${reset}\n"
printf "    LLVM debugger. Alternative to gdb, integrates well with clang output.\n\n"

printf "  ${green}cmake${reset}\n"
printf "    Portable build system generator. Describes your project in a\n"
printf "    CMakeLists.txt and generates Makefiles or Ninja files from it.\n"
printf "    Standard for multi-file C projects with external dependencies.\n\n"

printf "  ${green}ninja${reset}\n"
printf "    Fast build executor. Runs the build files cmake generates.\n"
printf "    Significantly faster than make on large incremental builds.\n\n"

printf "  ${green}valgrind${reset}\n"
printf "    Memory error detector. Runs your program in a sandboxed environment\n"
printf "    and reports every memory error: leaks, buffer overflows, use-after-free.\n"
printf "    Slow but thorough. Use it before every commit.\n\n"

printf "${byellow}  editor dependencies${reset}\n\n"

printf "  ${green}neovim${reset}\n"
printf "    The editor. Terminal-based, extensible, and fast.\n\n"

printf "  ${green}git${reset}\n"
printf "    Version control. Required by both NvChad and LazyVim to clone\n"
printf "    themselves and manage their plugins.\n\n"

printf "  ${green}ripgrep${reset}\n"
printf "    Fast text search across files. Used by Telescope inside Neovim\n"
printf "    for live grep across your project.\n\n"

printf "  ${green}fd${reset}\n"
printf "    Fast file finder. Used by Telescope for fuzzy file search.\n\n"

printf "  ${green}nodejs + npm${reset}\n"
printf "    JavaScript runtime and package manager. Required by some LSP\n"
printf "    servers such as bash-language-server.\n\n"

sleep 1

ask_yn "Install all packages listed above?"

if [[ "$reply" != "yes" ]]; then
    printf "${err_tag} ${bred}Installation declined.${reset} Nothing was changed.\n"
    exit 0
fi

printf "\n${ok_tag} ${bgreen}Accepted.${reset} Proceeding to installation...\n\n"
sleep 1


# =============================================================================
# SECTION 7 — SYSTEM UPGRADE
# Always upgrade before installing on Arch. The rolling release model means
# installing a package on top of an outdated system can cause dependency
# conflicts. -Syu syncs the package database (-y) and upgrades all packages (-u).
# =============================================================================

section_header "Step 1 — System Upgrade"

printf "${step_tag} Running ${cyan}sudo pacman -Syu${reset} to sync and upgrade the system.\n"
printf "${warn_tag} This may take a few minutes depending on how many updates are pending.\n\n"
sleep 1

run_cmd sudo pacman -Syu --noconfirm

printf "\n${ok_tag} System is up to date.\n"
sleep 1


# =============================================================================
# SECTION 8 — CORE TOOLCHAIN INSTALLATION
# Install the compiler, debugger, and build tools.
# --noconfirm skips the "Proceed with installation? [Y/n]" prompt since the
# user already confirmed above. $pacman_needed is empty unless --needed was
# passed, in which case it tells pacman to skip already-installed packages.
# =============================================================================

section_header "Step 2 — Core Toolchain"

printf "${step_tag} Installing compiler, debugger, and build tools...\n\n"
sleep 1

run_cmd sudo pacman -S --needed --noconfirm $pacman_needed \
    base-devel \
    gcc \
    gdb \
    clang \
    lldb \
    cmake \
    ninja \
    valgrind

printf "\n${step_tag} Verifying installations...\n\n"
sleep 1

# check that each binary is actually on PATH after install
check_cmd gcc
check_cmd gdb
check_cmd clang
check_cmd clangd     # ships with the clang package
check_cmd cmake
check_cmd ninja
check_cmd valgrind

printf "\n${ok_tag} ${bgreen}Toolchain installed and verified.${reset}\n"
sleep 1


# =============================================================================
# SECTION 9 — EDITOR DEPENDENCIES
# Install Neovim and the tools it depends on.
# =============================================================================

section_header "Step 3 — Editor Dependencies"

printf "${step_tag} Installing Neovim and its dependencies...\n\n"
sleep 1

run_cmd sudo pacman -S --needed --noconfirm $pacman_needed \
    neovim \
    git \
    ripgrep \
    fd \
    nodejs \
    npm

check_cmd nvim
check_cmd git
check_cmd rg
check_cmd fd
check_cmd node

printf "\n${ok_tag} ${bgreen}Editor dependencies installed.${reset}\n"
sleep 1


# =============================================================================
# SECTION 10 — EDITOR FRAMEWORK CHOICE
# Ask the user which Neovim framework they want: NvChad or LazyVim.
# Both are valid — NvChad is more opinionated and UI-polished out of the box,
# LazyVim is more modular and has official language extras including clangd.
# We present the tradeoffs and let the user decide.
# =============================================================================

section_header "Step 4 — Neovim Configuration"

printf "${info_tag} Choose your Neovim framework.\n\n"

printf "  ${bgreen}1) NvChad${reset}\n"
printf "     Beautiful UI out of the box. Fast startup. Ships with a clean default\n"
printf "     config and a built-in cheatsheet (Space+ch). LSP is configured manually\n"
printf "     in lspconfig.lua — you add server names to a list and they activate\n"
printf "     for their filetypes. This installer writes the clangd entry for you\n"
printf "     automatically using cat heredoc redirection.\n\n"

printf "  ${bgreen}2) LazyVim${reset}\n"
printf "     Built on lazy.nvim. Highly modular — language support is added by\n"
printf "     importing official extras. The C/clangd extra is a single line in\n"
printf "     lazy.lua and activates background indexing, clang-tidy integration,\n"
printf "     inlay hints, and a source/header switch keybinding. This installer\n"
printf "     writes that line for you automatically.\n\n"

# keep asking until we get a valid answer
editor_choice=""
while [[ "$editor_choice" != "1" && "$editor_choice" != "2" ]]; do
    printf "${ask_tag} Enter ${purple}1${reset} for NvChad or ${purple}2${reset} for LazyVim : "
    read -r editor_choice
done


# =============================================================================
# SECTION 11A — NVCHAD INSTALLATION
# Backs up any existing nvim config, clones NvChad, and writes the lspconfig
# file that enables clangd using cat heredoc redirection — no editor needed.
# =============================================================================

install_nvchad() {
    section_header "Installing NvChad"

    # back up existing neovim config if it exists
    # we do not delete it — the user may have spent time on it
    if [[ -d "$HOME/.config/nvim" ]]; then
        local backup="$HOME/.config/nvim.bak.$(date +%Y%m%d_%H%M%S)"
        printf "${warn_tag} Existing ${yellow}~/.config/nvim${reset} detected.\n"
        printf "${warn_tag} Moving it to ${yellow}${backup}${reset} as a backup.\n"
        run_cmd mv "$HOME/.config/nvim" "$backup"
    fi

    printf "${step_tag} Cloning NvChad starter into ${cyan}~/.config/nvim${reset}...\n\n"
    run_cmd git clone https://github.com/NvChad/starter "$HOME/.config/nvim" --depth 1

    printf "\n${ok_tag} NvChad cloned successfully.\n\n"
    sleep 1

    printf "${step_tag} Writing clangd LSP configuration...\n"
    printf "${info_tag} NvChad's lspconfig.lua lives at ${cyan}~/.config/nvim/lua/configs/lspconfig.lua${reset}\n"
    printf "${info_tag} We use cat with a heredoc to write the file — no editor required.\n\n"
    sleep 1

    # write the NvChad lspconfig.lua using cat heredoc redirection.
    # cat <<'EOF' > file redirects everything between the markers into the file.
    # the single-quoted 'EOF' prevents variable expansion inside the block —
    # the dollar signs in the Lua code are written literally, not interpreted by bash.
    #
    # NvChad modern (2024+) uses vim.lsp.enable() — not the old lspconfig[x].setup{}
    # pattern. servers listed here are activated for their respective filetypes.
    # clangd activates automatically for: c, cpp, objc, objcpp, cuda

    local lspconfig_path="$HOME/.config/nvim/lua/configs/lspconfig.lua"

    if [[ "$dry_run" == true ]]; then
        printf "${warn_tag} ${yellow}[dry-run]${reset} would write lspconfig.lua to: ${white}${lspconfig_path}${reset}\n"
    else
        mkdir -p "$(dirname "$lspconfig_path")"
        cat > "$lspconfig_path" <<'EOF'
-- =============================================================================
-- lspconfig.lua — NvChad LSP Configuration
-- Written by install.c.sh
-- =============================================================================
--
-- NvChad (2024+) uses vim.lsp.enable() to activate language servers.
-- Add a server name to the servers table and it will activate automatically
-- for all filetypes that server supports.
--
-- clangd activates for: .c .cpp .h .hpp .objc .cu files
-- It requires clangd to be installed: sudo pacman -S --needed clang
-- It uses compile_flags.txt or compile_commands.json as the project root.
--
-- To verify LSP is working: open a .c file and run :LspInfo
-- You should see: clangd (id: 1) · Attached buffers: 1
-- =============================================================================

-- load NvChad's default LSP settings (diagnostic signs, hover borders, etc.)
require("nvchad.configs.lspconfig").defaults()

-- ── server list ──────────────────────────────────────────────────────────────
-- add server names here as you install their binaries.
-- server name must match the nvim-lspconfig name exactly.
-- reference: https://github.com/neovim/nvim-lspconfig/blob/master/doc/configs.md
local servers = {
    "clangd",    -- C/C++ — sudo pacman -S --needed clang
    "lua_ls",    -- Lua (for editing nvim config) — sudo pacman -S --needed lua-language-server
    "bashls",    -- Bash — sudo npm install -g bash-language-server
    -- "rust_analyzer",  -- Rust — sudo pacman -S --needed rust-analyzer (uncomment when needed)
    -- "pyright",        -- Python — sudo pacman -S --needed pyright     (uncomment when needed)
}

-- activate all servers in the list
vim.lsp.enable(servers)

-- ── per-server custom settings (optional) ────────────────────────────────────
-- clangd runs with background indexing and clang-tidy integration.
-- these flags are passed directly to the clangd binary.
vim.lsp.config("clangd", {
    cmd = {
        "clangd",
        "--background-index",          -- indexes your project in the background
        "--clang-tidy",                -- runs clang-tidy checks as diagnostics
        "--header-insertion=iwyu",     -- suggests includes (include what you use)
        "--completion-style=detailed", -- show full type info in completions
        "--fallback-style=llvm",       -- code style when no .clang-format exists
    },
})
EOF
        printf "${ok_tag} Written: ${green}${lspconfig_path}${reset}\n"
    fi

    # drop a compile_flags.txt in the home directory as a starting point.
    # clangd searches upward from the opened file for this file to find the
    # project root. without it, root_dir is nil and some features are limited.
    local flags_path="$HOME/compile_flags.txt"
    if [[ ! -f "$flags_path" ]]; then
        write_file "$flags_path" "-std=c11"
        printf "${info_tag} Created ${green}~/compile_flags.txt${reset} with ${cyan}-std=c11${reset}\n"
        printf "${info_tag} Copy this file into any C project root so clangd knows the standard.\n"
    fi

    printf "\n${ok_tag} ${bgreen}NvChad configured.${reset}\n"
    printf "${info_tag} First launch: run ${cyan}nvim${reset} — plugins will install automatically.\n"
    printf "${info_tag} After plugins install, open a ${cyan}.c${reset} file and run ${cyan}:LspInfo${reset} to verify clangd.\n\n"
}


# =============================================================================
# SECTION 11B — LAZYVIM INSTALLATION
# Backs up existing config, clones LazyVim starter, and adds the clangd extra
# to lazy.lua using cat heredoc redirection. LazyVim's clangd extra is a single
# import line — it configures background indexing, clang-tidy, inlay hints,
# and switch source/header keybinding automatically.
# =============================================================================

install_lazyvim() {
    section_header "Installing LazyVim"

    # back up existing neovim config
    if [[ -d "$HOME/.config/nvim" ]]; then
        local backup="$HOME/.config/nvim.bak.$(date +%Y%m%d_%H%M%S)"
        printf "${warn_tag} Existing ${yellow}~/.config/nvim${reset} detected.\n"
        printf "${warn_tag} Moving it to ${yellow}${backup}${reset} as a backup.\n"
        run_cmd mv "$HOME/.config/nvim" "$backup"
    fi

    printf "${step_tag} Cloning LazyVim starter into ${cyan}~/.config/nvim${reset}...\n\n"
    run_cmd git clone https://github.com/LazyVim/starter "$HOME/.config/nvim" --depth 1

    # remove the .git directory so this can be tracked in your own dotfiles repo
    run_cmd rm -rf "$HOME/.config/nvim/.git"

    printf "\n${ok_tag} LazyVim cloned successfully.\n\n"
    sleep 1

    printf "${step_tag} Writing clangd extra into lazy.lua...\n"
    printf "${info_tag} LazyVim uses a single import line to activate the official clangd extra.\n"
    printf "${info_tag} The extra configures: background indexing, clang-tidy, inlay hints,\n"
    printf "${info_tag} detailed completions, and a ${cyan}<leader>ch${reset} switch source/header keybinding.\n\n"
    sleep 1

    # write lazy.lua — this is the LazyVim entry point where extras are imported.
    # the clangd extra is one import line:
    #   { import = "lazyvim.plugins.extras.lang.clangd" }
    # it pulls in the full official clangd configuration from LazyVim's repo.
    # ref: https://www.lazyvim.org/extras/lang/clangd

    local lazylua_path="$HOME/.config/nvim/lua/config/lazy.lua"

    if [[ "$dry_run" == true ]]; then
        printf "${warn_tag} ${yellow}[dry-run]${reset} would write lazy.lua to: ${white}${lazylua_path}${reset}\n"
    else
        mkdir -p "$(dirname "$lazylua_path")"
        cat > "$lazylua_path" <<'EOF'
-- =============================================================================
-- lazy.lua — LazyVim Entry Point
-- Written by install.c.sh
-- =============================================================================
--
-- This is where LazyVim loads its core plugins and all language extras.
-- Add extras by importing them in the spec table below.
--
-- The clangd extra activates for .c .cpp .h .hpp files and configures:
--   · clangd language server with background indexing
--   · clang-tidy diagnostics inline
--   · inlay hints (parameter names, return types)
--   · detailed completion with function argument placeholders
--   · <leader>ch to switch between .c and .h files
--   · codelldb DAP adapter (debugging with nvim-dap, optional)
--
-- Reference: https://www.lazyvim.org/extras/lang/clangd
-- =============================================================================

require("lazy").setup({
    spec = {
        -- ── LazyVim core ──────────────────────────────────────────────────
        -- loads LazyVim's built-in plugins: telescope, treesitter, lsp,
        -- completion, formatting, and the default UI.
        { "LazyVim/LazyVim", import = "lazyvim.plugins" },

        -- ── language extras ───────────────────────────────────────────────
        -- each import line activates official support for a language.
        -- clangd: C and C++ with full IDE features via clangd LSP.
        { import = "lazyvim.plugins.extras.lang.clangd" },

        -- uncomment these as you start working in other languages:
        -- { import = "lazyvim.plugins.extras.lang.rust"   },  -- Rust
        -- { import = "lazyvim.plugins.extras.lang.python" },  -- Python
        -- { import = "lazyvim.plugins.extras.lang.json"   },  -- JSON
        -- { import = "lazyvim.plugins.extras.lang.markdown" }, -- Markdown

        -- ── your own plugins ──────────────────────────────────────────────
        -- put your custom plugin specs in lua/plugins/*.lua
        -- lazy.nvim will pick them up automatically from that directory.
        { import = "plugins" },
    },

    -- ── lazy.nvim options ─────────────────────────────────────────────────
    defaults = {
        lazy = false,    -- load plugins at startup unless they set lazy = true
        version = false, -- always use the latest git commit, not pinned releases
    },
    install = {
        colorscheme = { "tokyonight", "habamax" },
    },
    checker = {
        enabled = true,  -- automatically check for plugin updates
    },
    performance = {
        rtp = {
            -- disable some built-in neovim plugins we do not need
            disabled_plugins = {
                "gzip", "tarPlugin", "tohtml", "tutor", "zipPlugin",
            },
        },
    },
})
EOF
        printf "${ok_tag} Written: ${green}${lazylua_path}${reset}\n"
    fi

    # same compile_flags.txt as NvChad — clangd needs a project root marker
    local flags_path="$HOME/compile_flags.txt"
    if [[ ! -f "$flags_path" ]]; then
        write_file "$flags_path" "-std=c11"
        printf "${info_tag} Created ${green}~/compile_flags.txt${reset} with ${cyan}-std=c11${reset}\n"
        printf "${info_tag} Copy this into any C project root so clangd finds the standard.\n"
    fi

    printf "\n${ok_tag} ${bgreen}LazyVim configured.${reset}\n"
    printf "${info_tag} First launch: run ${cyan}nvim${reset} — lazy.nvim installs all plugins automatically.\n"
    printf "${info_tag} After plugins load, open a ${cyan}.c${reset} file and run ${cyan}:LspInfo${reset} to verify clangd.\n\n"
}


# =============================================================================
# SECTION 12 — RUN THE CHOSEN EDITOR INSTALLER
# =============================================================================

if [[ "$editor_choice" == "1" ]]; then
    printf "${ok_tag} You chose: ${bgreen}NvChad${reset}\n\n"
    sleep 1
    install_nvchad
else
    printf "${ok_tag} You chose: ${bgreen}LazyVim${reset}\n\n"
    sleep 1
    install_lazyvim
fi


# =============================================================================
# SECTION 13 — SHELL ALIASES
# Write useful C development aliases to ~/.bashrc.
# Ask first — the user may manage their own dotfiles and not want us touching
# bashrc. If they say no, we print the aliases so they can add them manually.
# =============================================================================

section_header "Step 5 — Shell Aliases"

printf "${info_tag} The following aliases make C development faster.\n\n"

printf "  ${green}cc${reset}\n"
printf "    gcc -Wall -Wextra -Wpedantic -std=c11\n"
printf "    Compile with all warnings and strict C11 standard.\n"
printf "    Usage: cc -o prog prog.c\n\n"

printf "  ${green}ccd${reset}\n"
printf "    gcc -g -O0 -fsanitize=address,undefined -std=c11\n"
printf "    Compile with debug symbols, AddressSanitizer, and UBSan.\n"
printf "    Usage: ccd -o prog prog.c\n\n"

printf "  ${green}crun()${reset}\n"
printf "    Compile a single .c file and run it immediately.\n"
printf "    Usage: crun prog.c\n\n"

printf "  ${green}ec${reset}\n"
printf "    echo \"Exit: \$?\"\n"
printf "    Print the exit code of the last command.\n"
printf "    Usage: ./prog; ec\n\n"

sleep 1

ask_yn "Add these aliases to ~/.bashrc?"

if [[ "$reply" == "yes" ]]; then
    # check if aliases are already present to avoid duplicating them on re-runs
    # grep -q returns 0 (success) if the pattern is found, 1 if not
    if grep -q "# --- install.c.sh aliases ---" "$HOME/.bashrc" 2>/dev/null; then
        printf "${warn_tag} Aliases already present in ${yellow}~/.bashrc${reset} — skipping to avoid duplicates.\n"
    else
        if [[ "$dry_run" != true ]]; then
            # append the aliases block to .bashrc using cat heredoc redirection.
            # >> appends without overwriting. the marker comment lets us detect
            # duplicates on subsequent runs (see grep check above).
            cat >> "$HOME/.bashrc" <<'EOF'

# --- install.c.sh aliases ---
# C development shortcuts — written by install.c.sh

# compile with all warnings and strict C11 standard
alias cc='gcc -Wall -Wextra -Wpedantic -std=c11'

# compile with debug symbols, AddressSanitizer, and Undefined Behavior Sanitizer
# use this for all development builds — catches memory errors at runtime
alias ccd='gcc -g -O0 -fsanitize=address,undefined -std=c11'

# compile a single .c file and run it immediately
# usage: crun prog.c
function crun() {
    if [[ -z "$1" ]]; then
        printf "usage: crun <file.c>\n"
        return 1
    fi
    gcc -Wall -Wextra -Wpedantic -std=c11 -o /tmp/crun_out "$1" && /tmp/crun_out
}

# print the exit code of the last command
# useful for checking whether a program returned success (0) or failure (non-zero)
alias ec='echo "Exit: $?"'
# --- end install.c.sh aliases ---
EOF
        fi
        printf "${ok_tag} Aliases written to ${green}~/.bashrc${reset}\n"
        printf "${info_tag} Run ${cyan}source ~/.bashrc${reset} to activate them in the current shell.\n"
    fi
else
    printf "${info_tag} Skipped. Copy the aliases above manually into your shell config if you want them.\n"
fi


# =============================================================================
# SECTION 14 — VERIFY CLANGD VERSION
# Print the clangd version as a final sanity check.
# If clangd is not available, the LSP will not work regardless of config.
# =============================================================================

section_header "Step 6 — Final Verification"

printf "${step_tag} Verifying clangd...\n\n"
check_cmd clangd

printf "\n"
clangd --version

printf "\n${ok_tag} ${green}clangd${reset} is ready. It will activate when you open a ${cyan}.c${reset} file in nvim.\n"
sleep 1


# =============================================================================
# SECTION 15 — COMPLETION SUMMARY
# =============================================================================

section_header "Done"

printf "${done_tag} ${bgreen}Everything is installed and configured.${reset}\n\n"

printf "${byellow}  what was installed${reset}\n\n"
printf "  gcc clang gdb lldb cmake ninja valgrind    compiler toolchain\n"
printf "  neovim ripgrep fd nodejs npm               editor and dependencies\n"
printf "  clangd LSP config written via cat          IDE features in nvim\n"
printf "  shell aliases appended to ~/.bashrc        cc · ccd · crun · ec\n"

printf "\n${byellow}  what to do next${reset}\n\n"
printf "  1.  Run ${cyan}nvim${reset}\n"
printf "      Plugins install automatically on first launch.\n"
printf "      Wait for it to finish — do not quit early.\n\n"

printf "  2.  Open a .c file and run ${cyan}:LspInfo${reset}\n"
printf "      You should see clangd listed as an active client\n"
printf "      with ${green}Attached buffers: 1${reset}.\n\n"

printf "  3.  Run ${cyan}source ~/.bashrc${reset}\n"
printf "      To activate cc / ccd / crun in the current shell.\n\n"

printf "  4.  Drop a compile_flags.txt in any C project root:\n"
printf "      ${cyan}echo \"-std=c11\" > ~/myproject/compile_flags.txt${reset}\n"
printf "      This gives clangd a root to attach to.\n\n"

printf "  5.  Write your first program:\n"
printf "      ${cyan}crun hello.c${reset}\n\n"

printf "${info_tag} ${blue}https://rudv-ar.github.io/pages/Cpath.html#install${reset}\n\n"
printf "${done_tag} Good luck. Write something real.\n\n"

