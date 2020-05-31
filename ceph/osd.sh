#!/bin/bash

UUID=$(uuidgen)
ID=$(ceph osd create $UUID)

mkdir -p /var/lib/ceph/osd/ceph-$ID


mkfs.btrfs /dev/$1 -f

mount /dev/$1 /var/lib/ceph/osd/ceph-$ID

ceph-authtool --create-keyring /var/lib/ceph/osd/ceph-$ID/keyring \
                --name osd.$ID --add-key $OSD_SECRET

ceph-osd -i $ID --mkfs --mkkey  --osd-uuid $UUID
ceph auth add osd.$ID osd 'allow *' mon 'allow profile osd' -i /var/lib/ceph/osd/ceph-$ID/keyring
systemctl start ceph-osd@$ID
