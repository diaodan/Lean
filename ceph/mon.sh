#!/bin/bash
ceph-authtool --create-keyring /tmp/ceph.mon.keyring --gen-key -n mon. --cap mon 'allow *'
ceph-authtool --create-keyring /etc/ceph/ceph.client.admin.keyring --gen-key -n client.admin --set-uid=0 --cap mon 'allow *' --cap osd 'allow *' --cap mds 'allow'
ceph-authtool /tmp/ceph.mon.keyring --import-keyring /etc/ceph/ceph.client.admin.keyring

monmaptool --clobber --create --add ceph-node1 192.168.57.101 --fsid 7f8f00ac-c059-46a6-b656-4a1ce781607b /tmp/monmap
mkdir -p /var/lib/ceph/mon/ceph-node1
ceph-mon --mkfs -i ceph-node1 --monmap /tmp/monmap --keyring /tmp/ceph.mon.keyring
