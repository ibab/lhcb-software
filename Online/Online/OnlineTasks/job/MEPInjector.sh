#!/bin/bash
echo with extra arguments $*
#. ./setupOnline.sh
#. ../../GaudiOnline/cmt/setup.sh
#export gaudi_exe="$GAUDIONLINEROOT/$CMTCONFIG/Gaudi.exe $GAUDIONLINEROOT/$CMTCONFIG/libGaudiOnline.so GaudiOnline"

#export MSGSVC=MessageSvc
#${gaudi_exe} ${gaudi_exe_args} -opt=../options/MEPInjector.opts -msgsvc=${MSGSVC} -loop  
${GAUDIKERNELROOT}/../../InstallArea/$CMTCONFIG/bin/Gaudi.exe ../options/MEPInjector.opts
