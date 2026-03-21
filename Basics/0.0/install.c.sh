#!/bin/bash

# This is the ultimate setup guide for the C interface like a C compiler.
# ThisType = Automation Script

# Declarations -- define the color codes and variables

# colour variables — reset must always follow coloured text
reset='\033[0m'

# text colours
black='\033[0;30m'
red='\033[0;31m'
green='\033[0;32m'
yellow='\033[0;33m'
blue='\033[0;34m'
purple='\033[0;35m'
cyan='\033[0;36m'
white='\033[0;37m'

# bold variants
bred='\033[1;31m'
bgreen='\033[1;32m'
byellow='\033[1;33m'
bcyan='\033[1;36m'

install_prompt="${bcyan}[INSTALL.C | ${purple}install${bcyan}] > ${reset}Installing"
# Define the functions...

# Define the function to upgrade arch...
upgrade_arch() {
    printf "${install_prompt} ${green}upgrades${reset} for system..."

}


# Step 1; install the system -- full upgrade arch linux   
# -- using printf and not echo for a better escape char evaluation.

clear 
sleep 2 
printf "\n\n"
printf "${bcyan}[INSTALL.C] > ${reset} This is my own C installer setup which installs all necessary packages for a complete C development setup.\n"
sleep 2
printf "${bcyan}[INSTALL.C] > ${reset} The installer will include the following packages installed in this device.\n"
sleep 2 
printf "${bcyan}[INSTALL.C | ${byellow}warn${bcyan}] > ${reset} This system is assumed to be an arch based system, which uses ${green}pacman${reset} as the package manager as this script has all installers using ${green}pacman${reset}.\n\n"
echo ""
sleep 2
printf "${bcyan}[INSTALL.C | ${bgreen}packages${bcyan}] : ${reset} The following packages will be installed for the base setup...\n\n"
sleep 2

echo "
    base-devel : a package group containing gcc, make, binutils, etc - mandatory for a base setup. 
    gcc      : GNU C Compiler 
    gdb      : GNU C Debugger 
    clang    : alternative for gcc but with more detailed compile messages, llvm c compiler 
    lldb     : a linker for c 
    cmake    : the cmake makefile system used in projects - build file 
    ninja    : similar to cmake, but a faster linker 
    valgrind : the memory leak checker - use it daily to understand C deeply
" > ./packages.install.c.txt

less ./packages.install.c.txt 
sleep 2
rm ./packages.install.c.txt

printf "${bcyan}[INSTALL.C | ${byellow}(yes/no)${bcyan}] > ${reset} Do you want to install all these packages? : "
read confirm_packages

if [[ "$confirm_packages" == "yes" ]]; then
    printf "${bcyan}[INSTALL.C | ${bgreen}ACCEPTED${bcyan}] > ${reset} Entering package installation mode ..."
    sleep 2
    clear 
    upgrade_arch
else 
    printf "${bcyan}[INSTALL.C | ${bred}DECLINED${bcyan}] > ${reset} Declined installation...\n"

fi     
