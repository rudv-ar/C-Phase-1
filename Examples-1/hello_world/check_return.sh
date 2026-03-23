#!/bin/bash 

# Let's have a POC here...
# What can we do with a return from C?

echo "[+] Executing the hello_worldv2"
./hello_worldv2
result=$?

if [[ $result -eq 0 ]]; then
  echo "The return is 0; The command succeeded..."
else 
  echo "Non zero error code! $result"
fi

