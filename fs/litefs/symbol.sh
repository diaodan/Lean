#/bin/bash

echo -n "text address: "
cat /sys/module/$1/sections/.text
echo -n "data address: "
cat /sys/module/$1/sections/.data
echo -n "bss address : "
cat /sys/module/$1/sections/.bss
