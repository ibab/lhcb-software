#!/bin/bash
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
${GAUDIKERNELROOT}/../../InstallArea/$CMTCONFIG/bin/Gaudi.exe ${ONLINETASKSROOT}/options/MEPInjectorStdAlone.opts


#HOST=$(hostname --short | awk '{ print toupper($1) }')



#$BIGTERM MEPInjector@${HOST}   -e "
#export UTGID=${HOST}/INJECTOR

#. ./setupOnline.sh $*


#exec -a ${UTGID} ${gaudi_exe} -tasktype=LHCb::Class2Task -main=${GAUDIONLINEROOT}/options/Main.opts -opt=${ONLINETASKSROOT}/options/MEPInjector.opts




