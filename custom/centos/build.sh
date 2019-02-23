#/bin/bash

set -e

path=`pwd`
opt="$path/opt"
mnt="$path/mnt"
iso="CentOS-7-x86_64-DVD-1708.iso"
package=$path/rpm
time=`date +%F`

cleanroom="kde-l10n-French kde-l10n-German kde-l10n-Brazil kde-l10n-Ukrainian kde-l10n-Spanish kde-l10n-Russian kde-l10n-Polish kde-l10n-Swedish kde-l10n-Catalan"
cleanroom="${cleanroom} firefox libreoffice kde-l10n gimp-help gnome- texlive-"

if [ ! -e $path/$iso ]; then
    echo "No $iso file"
    exit 1
fi

rm -rf $opt
rm -rf $mnt

mkdir -p $opt
mkdir -p $mnt

mount -o loop $path/$iso $mnt

echo "Copy file ..."
rsync -av $mnt/ $opt/ > /dev/null

rm -rf $opt/repodata

if [ -d custom ]; then
    if [ -d postinstall ]; then
        cp -r postinstall $opt/
        cp -r custom $opt/
        cp isolinux.cfg $opt/isolinux
    fi
fi


umount $mnt
rm -rf $mnt

echo "copy rpm packages ..."
cp $package/* $opt/Packages
echo "clean no usage package ..."

for item in $cleanroom
do
    rm -f $opt/Packages/$item*
done

echo "Build repodata ..."
cd $opt
createrepo -g ../centos_comps.xml .

mkisofs -o ../CentOS-7-x86_64-DVD-1708-$time.iso -b isolinux/isolinux.bin \
        -c isolinux/boot.cat -no-emul-boot \
        -V 'CentOS 7 x86_64' \
        -boot-load-size 4 -boot-info-table -R -J -v -T .

cd ../
