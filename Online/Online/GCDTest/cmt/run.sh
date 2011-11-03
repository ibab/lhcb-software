#!/bin/bash
cd /home/frankm/cmtuser/Gaudi_v22r2/Online/GCDTest/cmt;
export CMTCONFIG=x86_64-slc5-gcc43-dbg;
. setup.${CMTCONFIG}.vars;
export SWEEP=sweep.cutoff-${2}musec;
mkdir -p /dev/shm/frankm/${SWEEP};
cd /dev/shm/frankm/${SWEEP};
#
#
#
/home/frankm/cmtuser/Gaudi_v22r2/Online/GCDTest/${CMTCONFIG}/lbGCDTest.exe -mult=${1} -num=10 -total=450 -thread=35 -wait=10 -out=sweep.${1}.root -cut=${2} -sweep>sweep.${1}.log;
#
#
#
cp sweep.${1}.log  /home/frankm/cmtuser/Gaudi_v22r2/Online/GCDTest/cmt/${SWEEP}/;
cp sweep.${1}.root /home/frankm/cmtuser/Gaudi_v22r2/Online/GCDTest/cmt/${SWEEP}/;
