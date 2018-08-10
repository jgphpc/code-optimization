#!/bin/bash

in=$1

if [ $in = gnu ];then
 for mm in -msse  -msse2  -msse3  -mssse3  -msse4.1  -msse4.2  -msse4  -mavx -mavx2 ;do
     make clean
     make NOCPU=1 VER=0 CXXFLAGS="-std=c++14 $mm -O2"
     mv exe $GNU_VERSION.$mm
     echo $GNU_VERSION / $mm
 done
fi

if [ $in = intel ];then
 for mm in -mavx -msse4.2 -msse4.1 -mssse3 -msse3  -msse2  -msse ;do
    make clean
    make NOCPU=1 VER=0 CXXFLAGS="-std=c++14 $mm -O2"
    mv exe $INTEL_VERSION.$mm
    echo $INTEL_VERSION / $mm
 done
fi


