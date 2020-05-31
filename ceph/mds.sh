#!/bin/bash
set -x

mkdir -p /var/lib/ceph/mds/ceph-$(hostname)
ceph-authtool --create-keyring /var/lib/ceph/mds/ceph-$(hostname)/keyring --gen-key -n mds.$(hostname)
ceph auth add mds.$(hostname) osd "allow rwx" mds "allow" mon "allow profile mds" \
                             -i /var/lib/ceph/mds/ceph-$(hostname)/keyring
