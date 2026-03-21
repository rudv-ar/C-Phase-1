
# INSTALL.C

This script is a bash script that installs all necessary stuffs that are required for a setup of complete `c programming interface`. By default, this script is only for arch based distributions and uses `pacman` to install things.
I wrote this script to automate the installation and that includes upgrading the system, installing packages and configuring the neovim editor.
Everything here is section wise installed with configuration options to choose between nvchad and lazyvim. This creates a backup of the current nvim config (if any in the system) with appropriate timestamp to make it easier to recover.

## Why?
I will program C in multiple devices and cannot afford to install each tool by hand in a fresh start.
