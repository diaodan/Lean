#/bin/bash

cat /sys/module/$1/sections/.text
cat /sys/module/$1/sections/.data
cat /sys/module/$1/sections/.bss
