#!/bin/bash

FULLNAME=${PACK_VERSION}
VERSION=${PACK_VERSION}-${PACK_RELEASE}

ARCH=`uname -i`
KERNEL=`uname -r`
DATE=`date +%F`

TMPDIR=${PACK_NAME}
TMPDDIR=${PACK_NAME}-${FULLNAME}
TMPFILE=${PACK_NAME}-${FULLNAME}.orig.tar.gz
DESTFILE=${PACK_NAME}-${FULLNAME}_${ARCH}.deb

build() {
    rm -rf $TMPDIR
    rm -f $TMPFILE
    build_make $1 $2
    build_debian $1 $2 $3
}

build_make() {
    pushd `pwd`
    cd $1
    make -j8
    make DESTDIR=$2 install
    popd
    cp -a ../kvm $2/usr/bin/
}

build_debian() {
    PWDOLD=`pwd`
    pushd `pwd`

    build_rules $1 $2 $3
    build_preinst $1 $2 $3
    build_postinst $1 $2 $3

    build_prerm $1 $2 $3 
    build_postrm $1 $2 $3
    build_copyright $1 $2 $3
    build_control $1 $2 $3
    build_changelog $1 $2 $3

    popd
}
build_preinst() {
    cat debian/preinst | sed -e "s/__PACK_NAME__/${PACK_NAME}/g" > $3/debian/preinst
}
build_postinst() {
    cat debian/postinst | sed -e "s/__PACK_NAME__/${PACK_NAME}/g" > $3/debian/postinst
}

build_prerm() {
    cat debian/prerm | sed -e "s/__PACK_NAME__/${PACK_NAME}/g" > $3/debian/prerm
}

build_postrm() {
    cat debian/postrm | sed -e "s/__PACK_NAME__/${PACK_NAME}/g" > $3/debian/postrm
}
build_copyright() {
    cat debian/copyright | sed -e "s/__PACK_NAME__/${PACK_NAME}/g" -e "s/__PACK_URL__/${PACK_URL}/g" -e "s/__PACK_VENDOR__/${PACK_VENDOR}/g" > $3/debian/copyright
}
build_control() {
    cat debian/control | sed -e "s/__PACK_NAME__/${PACK_NAME}/g" -e "s/__PACK_AUTHOR__/${PACK_AUTHOR}/g" -e "s/__PACK_AUTHOR_EMAIL__/${PACK_AUTHOR_EMAIL}/g" -e "s/__PACK_URL__/${PACK_URL}/g" -e "s/__PACK_COMPANY__/${PACK_COMPANY}/g" -e "s/__version__/${VERSION}/g" -e "s/__builddate__/${DATE}/g"  > $3/debian/control
}

build_changelog() {
    cat debian/changelog | sed -e "s/__PACK_NAME__/${PACK_NAME}/g" -e "s/__PACK_AUTHOR__/${PACK_AUTHOR}/g" -e "s/__PACK_AUTHOR_EMAIL__/${PACK_AUTHOR_EMAIL}/g" -e "s/__pack_version__/${VERSION}/g" > $3/debian/changelog
}


build_rules() {
    echo build_rulessdadad
    PWDOLD=`pwd`
    pushd `pwd`
    cat debian/rules | sed -e "s/__dest_file__/${DESTFILE}/g" -e "s/__PACK_NAME__/${PACK_NAME}/g" > $3/debian/rules

    cd $2

    DIRS=`find . -type d`
    FILES=`find . -type f`
    LINKS=`find . -type l`
    echo build_rulessdadad

    INSTALL="install:clean build\n"
    INSTALL="$INSTALL\tdh_testdir\n"
    INSTALL="$INSTALL\tdh_testroot\n\n"

    for item in $DIRS
    do
        item=`echo ${item#*.}`
        INSTALL="$INSTALL\tmkdir -p debian/$PACK_NAME/`echo ${item#*/}`\n"
    done

    INSTALL="$INSTALL\tdh_installdeb\n"
    INSTALL="$INSTALL\tdh_compress\n"
    INSTALL="$INSTALL\tdh_fixperms\n\n"

    for item in $FILES
    do
        MOD=`stat --format=%a $item`
        item=`echo ${item#*.}`
        DIRNAME=`dirname $item`
        INSTALL="$INSTALL\tinstall -m $MOD $PACK_NAME$item debian/$PACK_NAME$DIRNAME\n"
    done

    for item in $LINKS
    do
        item=`echo ${item#*.}`
        DIRNAME=`dirname $item`
        INSTALL="$INSTALL\tcp -a $PACK_NAME$item debian/$PACK_NAME/$DIRNAME\n"
    done
    echo build_rulessdadad

    INSTALL="$INSTALL""binary-indep:build install\n"
    INSTALL="$INSTALL""binary-arch:build install\n"
    INSTALL="$INSTALL\tdh_testdir\n"
    INSTALL="$INSTALL\tdh_testroot\n"
    INSTALL="$INSTALL\tdh_gencontrol\n"
    INSTALL="$INSTALL\tdh_md5sums\n"
    INSTALL="$INSTALL\tdh_builddeb --file ""$""{""DESTFILE""}""\n"

    INSTALL="$INSTALL""binary:binary-arch binary-indep\n"
    INSTALL="$INSTALL"".PHONY:clean build build-stamp install binary-indep binary-arch binary\n"

    echo -e $INSTALL >> $3/debian/rules
    popd
}

build_deb() {
    PWDOLD=`pwd`
    pushd `pwd`

    rm -rf $TMPDIR
    rm -rf $TMPDDIR
    mkdir -p $TMPDIR
    mkdir -p $TMPDDIR
    cp -a debian $TMPDDIR

    echo "tmpddir $TMPDDIR"
    build $1 `pwd`/$TMPDIR `pwd`/$TMPDDIR
    echo "after build"

    pushd `pwd`
    mv $TMPDIR $TMPDDIR
    cd $TMPDDIR
    echo "TMPDDIR"
    echo $TMPDDIR
    make -f debian/rules binary
    popd
    [ -d packages ] || mkdir -p packages
    cp ${DESTFILE} packages
    popd

}

build_deb $1


