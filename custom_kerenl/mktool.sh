#!/bin/bash

cd $1
find . | cpio -o -H newc | gzip -9 > ../initrd.img
