#/bin/bash

gdbtarget="target remote 192.168.122.1:1234"
gdbsymbol="add-symbol-file /root/Learn-master/fs/litefs/$1.ko "


gdbsymbol=$gdbsymbol`cat /sys/module/$1/sections/.text`
gdbsymbol=$gdbsymbol" -s .data "`cat /sys/module/$1/sections/.data`
gdbsymbol=$gdbsymbol" -s .bss "`cat /sys/module/$1/sections/.bss`

echo $gdbtarget
echo $gdbtarget > gdblitefs.ini
echo $gdbsymbol
echo $gdbsymbol >> gdblitefs.ini

scp gdblitefs.ini root@192.168.122.20:/root/linux-2.6.32.7/
