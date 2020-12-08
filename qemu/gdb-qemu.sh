#!/bin/bash
gdb --args x86_64-softmmu/qemu-system-x86_64  --enable-kvm -cpu host -m 4G -smp 2 -nographic \
    -drive if=pflash,format=raw,unit=0,file=/usr/share/OVMF/OVMF_CODE.fd,readonly \
    -drive file=/root/diskpool/debug_virtio/centos7-for-debug-kernerl.qcow2,format=qcow2 \
    -drive file=/root/diskpool/debug_virtio/qq.qcow2,format=qcow2,if=virtio \
    -netdev tap,id=infraservernet0,vhost=on,br=virbr0,ifname=infraservernet0 \
    -device virtio-net-pci,netdev=infraservernet0,mac=52:54:00:fc:21:81 \
    -device edu \
    -vnc :51 \
    -serial  /dev/ttyS1 > ./result 2>&1
