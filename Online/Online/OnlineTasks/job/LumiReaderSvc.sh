#!/bin/bash

if test -n "$1" ; then
   export UTGID=$1
fi


if [ -z $DIM_DNS_NODE ]; then
        echo "Please define DIM_DNS_NODE"
        exit 1
fi

#${GAUDIKERNELROOT}/../InstallArea/$CMTCONFIG/bin/Gaudi.exe ../options/MEPInjector.opts

cd /home/online/ONLINE/Online_v4r19/Online/OnlineTasks/job

HOST=$(hostname --short | awk '{ print toupper($1) }')

. ./setupOnline.sh $1

exec -a ${UTGID} ${gaudi_exe} -tasktype=LHCb::Class1Task -main=${GAUDIONLINEROOT}/options/Main.opts -opt=${ONLINETASKSROOT}/options/LumiReaderSvc.opts&

