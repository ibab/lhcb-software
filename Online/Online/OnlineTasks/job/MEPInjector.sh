#!/bin/bash

if test -n "$1" ; then
   export UTGID=$1
fi


if [ -z $DIM_DNS_NODE ]; then
        echo "Please define DIM_DNS_NODE"
        exit 1
fi

#${GAUDIKERNELROOT}/../InstallArea/$CMTCONFIG/bin/Gaudi.exe ../options/MEPInjector.opts

rm /dev/shm/*
#cd /home/online/ONLINE/Online_v4r20/Online/OnlineTasks/job

path=${0%/*}
cd $path

HOST=$(hostname --short | awk '{ print toupper($1) }')
export DEBUGGING=1
. ./setupOnline.sh $1

#export LD_LIBRARY_PATH=/sw/lib/lcg/external/gcc/4.3.2/x86_64-slc5/lib64/:$LD_LIBRARY_PATH

exec -a ${UTGID} ${gaudi_exe} -tasktype=LHCb::Class1Task -main=${GAUDIONLINEROOT}/options/Main.opts -opt=${ONLINETASKSROOT}/options/MEPInjector.opts&
