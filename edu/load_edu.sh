#!/bin/bash
module="edu"
device="edu"
mode="644"
/sbin/rmmod $module

rm -f /dev/${deivce}

/sbin/insmod ./$module.ko $* || exit 1


major=$(awk "\$2==\"$module\" {print \$1}" /proc/devices)

echo $major

mknod /dev/${device} c $major 0
