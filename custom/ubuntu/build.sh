#!/bin/bash
set -e
. ./require
path=`pwd`
iso="ubuntu-16.04.2-server-amd64.iso"
opt=$path/opt
mnt=$path/mnt
package=$path/deb
time=`date +%F`

if [ ! -e $path/$iso ]; then
    echo "No $iso file"
    exit 1
fi

rm -rf $opt
rm -rf $mnt
mkdir -p $opt
mkdir -p $mnt

mount -o loop $path/$iso $mnt

rsync -av $mnt/ $opt/ > /dev/null

umount $mnt
rm -rf $mnt

#chmod +w -R $opt/

late_command="d-i preseed/late_command string  cp /cdrom/lsb-release /target/etc/; cp /cdrom/$grub_pic /target/boot/grub; in-target /usr/sbin/update-grub2"
pkgsel="d-i	pkgsel/include string $require_deb $cloud_deb"

cp ./$grub_pic $opt
cat ./lsb-release | sed -e "s/DISTRIB_ID=Ubuntu/${distrib_id}/g" > $opt/lsb-release
echo $late_command >> $opt/preseed/ubuntu-server.seed
echo $pkgsel >> $opt/preseed/ubuntu-server.seed
if [ ! -d $package ]; then
    echo "No deb package dirtectory"
    exit 1
fi

cd $opt
if [ ! -d pool/restricted/c/cloud ]; then
    mkdir -p pool/restricted/c/cloud/
fi
cp $package/* pool/restricted/c/cloud/

dpkg-scanpackages pool/restricted /dev/null | gzip > dists/xenial/restricted/binary-amd64/Packages.gz

cd $opt
find . -type f -print0 | xargs -0 md5sum | grep -v "\./md5sum.txt" > md5sum.txt

mkisofs -r -V "xenial" -cache-inodes -J -l -b isolinux/isolinux.bin \
        -c isolinux/boot.cat -no-emul-boot -boot-load-size 4 -boot-info-table \
        -o ../ubuntu-16.04.2-server-amd64-$time.iso .
cd ../

