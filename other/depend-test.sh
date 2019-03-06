#!/bin/bash

OS_TYPE=CentOS
OS_TYPE=`cat /etc/issue | grep -q -i CentOS && echo "CentOS" || echo "$OS_TYPE"`
OS_TYPE=`cat /etc/issue | grep -q -i Ubuntu && echo "Ubuntu" || echo "$OS_TYPE"`

rpm_conflict_libvirt_pack=""
rpm_conflict_qemu_pack=""
rpm_conflict_openvswitch_pack=""

conflict_test_rpm() {
    rpm_files=`rpm -qpl $1`
    for item in $rpm_files
    do
        if [ -d $item ]
        then 
            continue
        elif [ -f $item ]
        then
            conflict_rpm=`rpm -qf $item`
            if [ $? -eq 0 ]
            then
                echo soft $conflict_rpm conflict $1 file $item
                echo please purge uninstall $conflict_rpm
                exit 1
            fi
        fi
    done

    libvirt=`echo $1 | grep libvirt`
    if [ $? -eq 0 ]
    then
        for item in $rpm_conflict_libvirt_pack
        do
            stat=`rpm -q $item 2>/dev/null`
            if [ $? -eq 0 ]
            then
                echo "soft $item conflict $1"
                echo "please purge uninstall $item"
                exit 1
            fi
        done
    fi

    qemu=`echo $1 | grep qemu`
    if [ $? -eq 0 ]
    then
        for item in $rpm_conflict_qemu_pack
        do
            stat=`rpm -q $item 2>/dev/null`
            if [ $? -eq 0 ]
            then
                echo "soft $item conflict $1"
                echo "please purge uninstall $item"
                exit 1
            fi
        done
    fi

    openvswitch=`echo $1 | grep openvswitch`
    if [ $? -eq 0 ]
    then
        for item in $rpm_conflict_openvswitch_pack
        do
            stat=`rpm -s $item 2>/dev/null`
            if [ $? -eq 0 ]
            then
                echo "soft $item conflict $1"
                echo "please purge uninstall $item"
                exit 1
            fi
        done
    fi

    echo "conflict test success"
}

deb_conflict_libvirt_pack="libvirt0"
deb_conflict_qemu_pack=""
deb_conflict_openvswitch_pack=""

conflict_test_deb() {
    deb_files=`dpkg -X $1 /`
    for item in $deb_files
    do 
        item=`echo ${item#*.}`
        if [ -d $item ]
        then
            continue
        elif [ -f $item ]
        then
            conflict_deb=`dpkg -S $item 2>/dev/null`
            if [ $? -eq 0 ]
            then
                echo soft $conflict_deb conflict $1 file $item
                conflict_deb=`echo ${conflict_deb%:*}`
                echo please purge uninstall $conflict_deb
                exit 1
            fi
        fi
    done

    libvirt=`echo $1 | grep libvirt`
    if [ $? -eq 0 ]
    then
        for item in $deb_conflict_libvirt_pack
        do
            stat=`dpkg -s $item 2>/dev/null`
            if [ $? -eq 0 ]
            then
                echo "soft $item conflict $1"
                echo "please purge uninstall $item"
                exit 1
            fi
        done
    fi

    qemu=`echo $1 | grep qemu`
    if [ $? -eq 0 ]
    then
        for item in $deb_conflict_qemu_pack
        do
            stat=`dpkg -s $item 2>/dev/null`
            if [ $? -eq 0 ]
            then
                echo "soft $item conflict $1"
                echo "please purge uninstall $item"
                exit 1
            fi
        done
    fi

    openvswitch=`echo $1 | grep openvswitch`
    if [ $? -eq 0 ]
    then
        for item in $deb_conflict_openvswitch_pack
        do
            stat=`dpkg -s $item 2>/dev/null`
            if [ $? -eq 0 ]
            then
                echo "soft $item conflict $1"
                echo "please purge uninstall $item"
                exit 1
            fi
        done
    fi
    echo "conflict test success"
}

#Centos libvirt qemu openvswitch
rpm_libutil=(glibc 2.17 /lib64/libutil.so.1)
rpm_libdl=(glibc 2.17 /lib64/libdl.so.2)
rpm_libpthread=(glibc 2.17 /lib64/libpthread.so.0)
rpm_libm=(glibc 2.17 /lib64/libm.so.6)
rpm_librt=(glibc 2.17 /lib64/librt.so.1)
rpm_libudev=(systemd-libs 219 /lib64/libudev.so.1)
rpm_libz=(zlib 1.2.7 /lib64/libz.so.1)
rpm_liblzma=(xz-libs 5.2.2 /lib64/liblzma.so.5)
rpm_libpcre=(pcre 8.32 /lib64/libpcre.so.1)
rpm_libcap=(libcap 2.22 /lib64/libcap.so.2)
rpm_libdw=(elfutils-libs 0.168 /lib64/libdw.so.1)
rpm_libgcc_s=(libgcc 4.8.5 /lib64/libgcc_s.so.1)
rpm_libattr=(libattr 2.4.46 /lib64/libattr.so.1)
rpm_libelf=(elfutils-libelf 0.168 /lib64/libelf.so.1)
rpm_libbz2=(bzip2-libs 1.0.6 /lib64/libbz2.so.1)

#libvirtd
rpm_libyajl=(yajl 2.0.4 /lib64/libyajl.so.2)
rpm_libnlroute3=(libnl3 3.2.28 /lib64/libnl-route-3.so.200)
rpm_libnl3=(libnl3 3.2.28 /lib64/libnl-3.so.200)
rpm_libdevmapper=(device-mapper-libs 1.02.140 /lib64/libdevmapper.so.1.02)
rpm_libxml2=(libxml2 2.9.1 /lib64/libxml2.so.2)
rpm_libselinux=(libselinux 2.5 /lib64/libselinux.so.1)
rpm_libc=(glibc 2.17 /lib64/libc.so.6)
rpm_libsepol=(libsepol 2.5 /lib64/libsepol.so.1)
#tool
rpm_pm=(pm-utils 1.4.1 /usr/bin/pm-is-supported)

#qemu
rpm_libusb=(libusbx 1.0.20 /lib64/libusb-1.0.so.0)
rpm_libgthread=(glib2 2.50.3 /lib64/libpthread.so.0)
rpm_libglib=(glib2 2.50.3 /lib64/libglib-2.0.so.0)
rpm_libstdc=(libstdc++ 4.8.5 /lib64/libstdc++.so.6)
 
#openvswitch
o_rpm_libc=(glibc 2.17 /lib64/libc.so.6)

rpm_package=(${rpm_libutil[*]} ${rpm_libdl[*]} ${rpm_libpthread[*]} ${rpm_libm[*]} ${rpm_librt[*]} ${rpm_libudev[*]} \
         ${rpm_libz[*]} ${rpm_liblzma[*]} ${rpm_libpcre[*]} ${rpm_libcap[*]} ${rpm_libdw[*]} ${rpm_libgcc_s[*]} \
         ${rpm_libattr[*]} ${rpm_libelf[*]} ${rpm_libbz2[*]} \
         ${rpm_libyajl[*]} ${rpm_libnlroute3[*]} ${rpm_libnl3[*]} ${rpm_libdevmapper[*]} ${rpm_libxml2[*]} \
         ${rpm_libselinux[*]} ${rpm_libc[*]} ${rpm_libsepol[*]} \
         ${rpm_libusb[*]} ${rpm_libgthread[*]} ${rpm_libglib[*]} ${rpm_libstdc[*]} ${rpm_pm[*]} \
         ${o_rpm_libc[*]})

depend_test_rpm() {
    var1=0
    for item in ${rpm_package[*]}
    do
        var2=$[$var1 + 1]
        var1=$[var2] 
    done

    echo "require" $[$var2/3]

    for ((i=0; i<$var2; i+=3))
    do
        echo -n ${rpm_package[$i]}
        version=`rpm -q --queryformat=%{version} ${rpm_package[$i]}`
        if [ $? -eq 0 ]
        then
            echo " version "$version
        else
            echo " no install, please install ${rpm_package[$i]}"
            exit 1
        fi
    done
    echo "depend test success"
}

#Ubuntu
#libvirt and qemu openvswitch
deb_libz=(zlib1g 1:1.2.8.dfsg-2ubuntu4.1 libz.so.1)
deb_libutil=(libc6 2.23-0ubuntu9 libutil.so.1)
deb_libm=(libc6 2.23-0ubuntu9 libm.so.6)
deb_libgcc_s=(libgcc1 1:6.0.1-0ubuntu1 libgcc_s.so.1)
deb_libpthread=(libc6 2.23-0ubuntu9 libpthread.so.0)
deb_libc=(libc6 2.23-0ubuntu9 libc.so.6)
deb_libudev=(libudev1 229-4ubuntu21 libudev.so.1)
deb_libpcre=(libpcre3 2:8.28-3.1 libpcre.so.3)
deb_librt=(libc6 2.23-0ubuntu9 librt.so.1)

#libvirt
deb_libselinux=(libselinux1 2.4-3build2 libselunyx.so.1)
deb_libyajl=(libyajl2 2.1.0-2 libyajl.so.2)
deb_libnl3=(libnl-3-200 3.2.27-1ubuntu0.16.04.1 libnl-3.so.200)
deb_libdevmapper=(libdevmapper1.02.1 2:1.02.110-1ubuntu10 libdevmapper.so.1.02.1)
deb_libxml2=(libxml2 2.9.3+dfsg1-1ubuntu0.2 libxml2.so.2)
deb_libdbus=(libdbus-1-3 1.10.6-1ubuntu3.3 libdbus-1.so.3)
deb_libdl=(libc6 2.23-0ubuntu9 libdl.so.2)
deb_libicuuc=(libicu55 55.1-7ubuntu0.2 libicuuc.so.55)
deb_liblzma=(liblzma5 5.1.1alpha+20120614-2ubuntu2 liblzma.so.5)
deb_libsystemd=(libsystemd0 229-4ubuntu16 libsystemd.so.0)
deb_libicudata=(libicu55 55.1-7ubuntu0.2 libciudata.so.55)
deb_libstdc=(libstdc++6 5.4.0-6ubuntu1~16.04.4 libstdc++.so.6)
deb_libgcrypt=(libgcrypt20 1.6.5-2ubuntu0.2 libgcrypt.so.20)
deb_libgpgerror=(libgpg-error0 1.21-2ubuntu1 libgpg-error.so.0)

#qemu
deb_libpixman=(libpixman-1-0 0.33.6-1 libpixman-1.so.0)
deb_libusb=(cloud-libusb 2:1.0.20-1 libusb-1.0.so.0)
deb_libglib=(libglib2.0-0 2.48.2-0ubuntu1 libglib-2.0.so.0)

#tool
dep_pm=(pm-utils 1.4.1-16 pm-is-supported)

#openvswitch
o_deb_libssl=(libssl1.0.0 1.0.2g-1ubuntu4.8 libssl.so.1.0.0)
o_deb_libcrypto=(libssl1.0.0 1.0.2g-1ubuntu4.8 libcrypto.so.1.0.0)
o_deb_libdl=(libc6 2.23-0ubuntu9 libdl.so.2)

deb_pack=(${deb_libz[*]} ${deb_libutil[*]} ${deb_libm[*]} ${deb_libgcc_s[*]} ${deb_libpthread[*]} \
      ${deb_libc[*]} ${deb_libudev[*]} ${deb_libpcre[*]} \
      ${deb_libselinux[*]} ${deb_libyajl[*]} ${deb_libnl3[*]} ${deb_libdevmapper[*]} ${deb_libxml2[*]} \
      ${deb_libdbus[*]} ${deb_libdl[*]} ${deb_librt[*]} ${deb_libicuuc[*]} ${deb_liblzma[*]} ${deb_libsystemd[*]} \
      ${deb_libicudata[*]} ${deb_libstdc[*]} ${deb_libgcrypt[*]} ${deb_libgpgerror[*]} \
      ${deb_libpixman[*]} ${deb_libusb[*]} ${deb_libglib[*]} ${deb_librt[*]} ${dep_pm[*]} \
      ${o_deb_libssl[*]} ${o_deb_libcrypto[*]} ${o_deb_libdl[*]})

depend_test_deb() {
    var1=0
    var2=0

    for item in ${deb_pack[*]}
    do
        var2=$[$var1 + 1]
        var1=$[var2]
    done

    echo "require" $[$var2/3]

    for ((i=0; i<$var2; i+=3))
    do
        echo -n ${deb_pack[$i]}
        stat=`dpkg --get-selections ${deb_pack[$i]}`
        if [ $? -eq 0 ]
        then
            install=`echo $stat | awk '{print $2}'`
            if [ "$install" = "install" ]
            then
                version=`dpkg -s ${deb_pack[$i]} 2>/dev/null | grep Version`
                version=`echo $version | sed 's/Version: //g'`
                echo " version" $version" " $install
            else
                echo " not install ${deb_pack[$i]}, please install ${deb_pack[$i]}"
                exit 1
            fi
        else
            echo " not install ${deb_pack[$i]}, please install ${deb_pack[$i]}"
            exit 1
        fi
    done
    echo "depend test success"
}

case $1 in
    depend)
        if [ $OS_TYPE = "Ubuntu" ]
        then
            echo "Ubuntu"
            depend_test_deb
        elif [ $OS_TYPE = "CentOS" ]
        then
            echo "CentOS"
            depend_test_rpm
        else
            echo "Unknow system"
        fi;;
    conflict)
        if [ -z $2 ]
        then 
            echo "need package args"
            exit 1
        fi

        if [ $OS_TYPE = "Ubuntu" ]
        then
            echo "Ubuntu"
            conflict_test_deb $2
        elif [ $OS_TYPE = "CentOS" ]
        then
            echo "CentOS"
            conflict_test_rpm $2
        else
            echo "Unknow system"
        fi;;
    *)
        echo "usage : $0 args"
        echo "args:"
        echo "      depend                  test libvirt/qemu dependence package"
        echo "      conflict x.deb/x.rpm    test deb/rpm conflict package"
        exit 1;;
esac
