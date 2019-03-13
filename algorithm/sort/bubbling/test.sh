#!/bin/bash

echo "gsort1"
time ./gsort1 $1 /datapool1/lilu/iso/CentOS.7.2.64.iso > tmp
echo "gsort2"
time ./gsort2 $1 /datapool1/lilu/iso/CentOS.7.2.64.iso > tmp
echo "gsort3"
time ./gsort3 $1 /datapool1/lilu/iso/CentOS.7.2.64.iso > tmp



echo "sort"
time ./sort $1 /datapool1/lilu/iso/CentOS.7.2.64.iso > tmp
echo "sort1"
time ./sort1 $1 /datapool1/lilu/iso/CentOS.7.2.64.iso > tmp
echo "sort2"
time ./sort2 $1 /datapool1/lilu/iso/CentOS.7.2.64.iso > tmp
echo "sort3"
time ./sort3 $1 /datapool1/lilu/iso/CentOS.7.2.64.iso > tmp
