#!/bin/bash

OS_TYPE=CentOS
OS_TYPE=`cat /etc/issue | grep -q -i CentOS && echo "CentOS" || echo "$OS_TYPE"`
OS_TYPE=`cat /etc/issue | grep -q -i Ubuntu && echo "Ubuntu" || echo "$OS_TYPE"`

export PACK_NAME="cloud-qemu"
export PACK_VERSION=2.9.0
export PACK_RELEASE=1
export PACK_AUTHOR="lilu"
export PACK_AUTHOR_EMAIL="lilu@example.com.cn"
export PACK_COMPANY="example"
export PACK_URL="www.example.com.cn"
export PACK_VENDOR="example"

if [ $OS_TYPE = "CentOS" ]
then
    pushd `pwd`
    cd rpm
    ./buildrpm.sh $1
    popd
    [ -d packages ] || mkdir -p packages
    cp rpm/packages/* packages/
elif [ $OS_TYPE = "Ubuntu" ]
then
    pushd `pwd`
    cd deb
    ./builddeb.sh $1
    popd
    [ -d packages ] || mkdir -p packages
    cp deb/packages/* packages/
else
    echo "Unknown system"
    exit 1
fi
