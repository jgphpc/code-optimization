#!/bin/bash

in=$1

    compiler_version=`echo $in |cut -d_ -f2 |cut -d- -f1`
    compiler_flag=`echo $in |cut -d- -f2`
    compiler_time=`grep "Total Time " $in |awk '{print $6}'`
    echo $compiler_version $compiler_flag $compiler_time |awk '{printf "%15s %8s %s\n",$1,$2,$3}'
    
exit 0

    18.0.2.199.     mavx 9.9688
    18.0.2.199.  msse4.1 12.883
    18.0.2.199.  msse4.2 12.997
    18.0.2.199.   mssse3 14.244
    18.0.2.199.    msse3 14.251
    18.0.2.199.     msse 14.255
    18.0.2.199.    msse2 14.26

         7.3.0.  msse4.1 33.721
         7.3.0.    msse4 33.724
         7.3.0.   mssse3 33.744
         7.3.0.    msse3 33.822
         7.3.0.    msse2 33.828
         7.3.0.     mavx 33.849
         7.3.0.  msse4.2 33.857
         7.3.0.    mavx2 33.921
         7.3.0.     msse 33.984

         4.9.3.     mavx 34.132
         4.9.3.    msse3 34.279
         4.9.3.    msse4 34.282
         4.9.3.     msse 34.283
         4.9.3.  msse4.2 34.298
         4.9.3.    mavx2 34.317
         4.9.3.    msse2 34.377
         4.9.3.  msse4.1 34.4
         4.9.3.   mssse3 34.404

         5.3.0.    mavx2 34.562
         5.3.0.  msse4.1 34.687
         5.3.0.     mavx 34.69
         5.3.0.     msse 34.695
         5.3.0.    msse4 34.7
         5.3.0.   mssse3 34.731
         5.3.0.    msse3 34.762
         5.3.0.  msse4.2 34.799
         5.3.0.    msse2 34.83

         6.2.0.   mssse3 48.909
         6.2.0.     msse 48.934
         6.2.0.    msse4 48.941
         6.2.0.    msse2 48.943
         6.2.0.    msse3 48.946
         6.2.0.  msse4.1 48.97
         6.2.0.  msse4.2 49.014
         6.2.0.     mavx 49.061
         6.2.0.    mavx2 49.102
