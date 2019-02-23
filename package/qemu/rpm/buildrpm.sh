#!/bin/bash

FULLNAME=${PACK_VERSION}
VERSION=${PACK_VERSION}-${PACK_RELEASE}

DATE=`date +%F`
ARCH=`uname -r`

TMPDIR=${PACK_NAME}
TMPDDIR=${PACK_NAME}-${FULLNAME}
TMPFILE=${PACK_NAME}-${FULLNAME}.tar.gz
DESTFILE=${PACK_NAME}-${FULLNAME}-${ARCH}.rpm
PACKAGE_REQUIRES="rpm"


build() {
    rm -rf $TMPDIR
    rm -f $TMPFILE
    build_make $1 $2
    build_spec $1 $2
}

build_make() {
    pushd `pwd`
    cd $1
    make -j8
    make DESTDIR=$2 install
    popd
    cp -a ../kvm $2/usr/bin/
}


build_spec() {
    PWDOLD=`pwd`
    pushd `pwd`
    cat $PWDOLD/soft.spec | sed -e "s/__PACK_VERSION__/${PACK_VERSION}/g" -e "s/__PACK_RELEASE__/${PACK_RELEASE}/g" -e "s/__KERNEL_ARCH__/${ARCH}/g" -e "s/__PACKAGE_REQUIRES__/${PACKAGE_REQUIRES}/g" -e "s/__BUILD_DATE__/${DATE}/g" -e "s/__PACK_NAME__/${PACK_NAME}/g" -e "s/__PACK_VENDOR__/${PACK_VENDOR}/g" -e "s/__PACK_URL__/${PACK_URL}/g" -e "s/__PACK_COMPANY__/${PACK_COMPANY}/g"  > $PWDOLD/rpmbuild/SPECS/soft.spec

    build_install $1 $2
    build_files $1 $2
    build_pre
    build_post
    build_preun
    build_postun

    echo "%changelog" >> $PWDOLD/rpmbuild/SPECS/soft.spec
    popd
}

build_install() {
    echo install pwd `pwd`
    PWDOLD=`pwd`
    pushd `pwd`
    cd $2

    DIRS=`find . -type d`
    FILES=`find . -type f`
    LINKS=`find . -type l`

    INSTALL="%install\n"
    INSTALL="$INSTALL\nrm -rf %{buildroot}\n"

    for item in $DIRS
    do
        item=`echo ${item#*.}`
        INSTALL="$INSTALL \nmkdir -p %{buildroot}/`echo ${item#*/}`"
    done

    for item in $FILES
    do
        MOD=`stat --format=%a $item`
        item=`echo ${item#*.}`
        DIRNAME=`dirname $item`
        INSTALL="$INSTALL \ninstall -m $MOD $PACK_NAME$item %{buildroot}$DIRNAME"
    done

    for item in $LINKS
    do
        item=`echo ${item#*.}`
        MOD=`stat --format=%a $item`
        DIRNAME=`dirname $item`
        INSTALL="$INSTALL \ncp -a $PACK_NAME$item %{buildroot}$DIRNAME"
    done

    echo pwd $PWDOLD
    echo -e $INSTALL >> $PWDOLD/rpmbuild/SPECS/soft.spec
    popd
}

build_files() {
    PWDOLD=`pwd`
    pushd `pwd`
    cd $2

    FILES=`find . -type f`
    LINKS=`find . -type l`

    LIST=""

    for item in $FILES
    do
        echo $item | grep "/man/"
        if [ $? -eq 0 ]
        then
            item=`echo ${item#*.}`
            LIST="$LIST\n$item.gz"
        else
            item=`echo ${item#*.}`
            LIST="$LIST\n$item"
        fi
    done

    for item in $LINKS
    do
        item=`echo ${item#*.}`
        LIST="$LIST\n$item"
    done

    echo "%files" >> $PWDOLD/rpmbuild/SPECS/soft.spec
    echo "%defattr(-,root,root)" >> $PWDOLD/rpmbuild/SPECS/soft.spec
    echo -e $LIST >> $PWDOLD/rpmbuild/SPECS/soft.spec

    popd
}

build_pre() {
    PWDOLD=`pwd`
    pushd `pwd`
    echo "%pre" >> $PWDOLD/rpmbuild/SPECS/soft.spec
    echo "echo 'Begin install $PACK ...'" >> $PWDOLD/rpmbuild/SPECS/soft.spec
    popd
}

build_post() {
    PWDOLD=`pwd`
    pushd `pwd`
    echo "%post" >> $PWDOLD/rpmbuild/SPECS/soft.spec
    echo "echo 'Install $PACK success'" >> $PWDOLD/rpmbuild/SPECS/soft.spec
    popd
}

build_preun() {
    PWDOLD=`pwd`
    pushd `pwd`
    echo "%preun" >> $PWDOLD/rpmbuild/SPECS/soft.spec
    echo "echo 'Begin uninstall $PACK success'" >> $PWDOLD/rpmbuild/SPECS/soft.spec
    popd
}

build_postun() {
    PWDOLD=`pwd`
    pushd `pwd`
    echo "%postun" >> $PWDOLD/rpmbuild/SPECS/soft.spec
    echo "if [ ""$""1 -eq 0 ]; then" >> $PWDOLD/rpmbuild/SPECS/soft.spec
    echo "  echo 'Uninstall $PACK success'" >> $PWDOLD/rpmbuild/SPECS/soft.spec
    echo "fi" >> $PWDOLD/rpmbuild/SPECS/soft.spec
    popd
}

build_rpm() {
    PWDOLD=`pwd`
    pushd `pwd`

    mkdir -p $TMPDIR
    [ -d package ] || mkdir packages

    rm -rf ~/.rpmmacros
    echo "%_topdir `pwd`/rpmbuild" > ~/.rpmmacros
    echo pwd `pwd`
    mkdir -p rpmbuild/BUILD rpmbuild/RPMS/$ARCH rpmbuild/SOURCES rpmbuild/SPECS rpmbuild/SRPMS
    build $1 `pwd`/$TMPDIR
    tar czf $TMPFILE $TMPDIR
    cp $TMPFILE rpmbuild/SOURCES

    rpmbuild --bb --buildroot `pwd`/rpmbuild/root rpmbuild/SPECS/soft.spec
    RPMFILE=`find $PWD/rpmbuild/RPMS/ | grep "${PACK_NAME}-${VERSION}"`
    cp $RPMFILE packages/${DESTFILE}
    

}

build_rpm $1
