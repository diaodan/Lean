#!/bin/bash
module="scull"
device="scull"
mode="644"
/sbin/rmmod $module

rm -f /dev/${deivce}[0-3]
rm -f /dev/scull0
rm -f /dev/scull1
rm -f /dev/scull2
rm -f /dev/scull3

/sbin/insmod ./$module.ko $* || exit 1


major=$(awk "\$2==\"$module\" {print \$1}" /proc/devices)

echo $major

mknod /dev/${device}0 c $major 0
mknod /dev/${device}1 c $major 1
mknod /dev/${device}2 c $major 2
mknod /dev/${device}3 c $major 3
