#!/bin/bash
#qemu-system-x86_64 -kernel ./bzImage -initrd ./initrd.img -m 512 -smp 1 -net nic -vnc :20 -gdb tcp::1234 -S
qemu-system-x86_64 -kernel ./bzImage -initrd ./initrd.img -m 4096 -smp 4 -net nic -vnc :21 -gdb tcp::1234 -S
