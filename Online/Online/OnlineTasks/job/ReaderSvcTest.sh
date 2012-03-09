#!/bin/bash

if test -n "$1" ; then
   export UTGID=$1
fi

if [ -z $DIM_DNS_NODE ]; then
        echo "Please define DIM_DNS_NODE"
        exit 1
fi

path=${0%/*}
cd $path

HOST=$(hostname --short | awk '{ print toupper($1) }')
export DEBUGGING=1
. ./setupOnline.sh $1


exec -a ${UTGID} ${gaudi_exe} -tasktype=LHCb::Class2Task -main=${GAUDIONLINEROOT}/options/Main.opts -opt=${ONLINETASKSROOT}/options/ReaderSvcTest.opts&
#exec -a ${UTGID} valgrind --leak-check=yes --tool=memcheck --show-reachable=yes --track-fds=yes ${gaudi_exe} -tasktype=LHCb::Class2Task -main=${GAUDIONLINEROOT}/options/Main.opts -opt=${ONLINETASKSROOT}/options/ReaderSvcTest.opts&

