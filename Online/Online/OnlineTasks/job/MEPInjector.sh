#!/bin/bash

if test -n "$1" ; then
   export UTGID=$1
fi


cd /home/online/ONLINE/Online_v4r18dev/Online/OnlineTasks/job


if [ -z $DIM_DNS_NODE ]; then
        echo "Please define DIM_DNS_NODE"
        exit 1
fi


#. ./setupOnline.sh
#. ../../GaudiOnline/cmt/setup.sh
#export gaudi_exe="$GAUDIONLINEROOT/$CMTCONFIG/Gaudi.exe $GAUDIONLINEROOT/$CMTCONFIG/libGaudiOnline.so GaudiOnline"


#${gaudi_exe} ${gaudi_exe_args} -opt=../options/MEPInjector.opts -msgsvc=${MSGSVC} -loop  
#valgrind -v --leak-check=yes  
#${GAUDIKERNELROOT}/../../InstallArea/$CMTCONFIG/bin/Gaudi.exe ../options/MEPInjector.opts 
#valgrind --tool=callgrind 
#${GAUDIKERNELROOT}/../../InstallArea/$CMTCONFIG/bin/Gaudi.exe ${ONLINETASKSROOT}/options/MEPInjector.opts


HOST=$(hostname --short | awk '{ print toupper($1) }')

#$BIGTERM MEPInjector@${HOST}   -e "
#export UTGID=${HOST}_Injector_1

echo $ONLINETASKSROOT
. ./setupOnline.sh $*

echo ${gaudi_exe}

exec -a ${UTGID} ${gaudi_exe} -tasktype=LHCb::Class1Task -main=${GAUDIONLINEROOT}/options/Main.opts -opt=${ONLINETASKSROOT}/options/MEPInjector.opts&




