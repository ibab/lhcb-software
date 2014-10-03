#!/bin/bash
echo "Hello from MEPInjector: $*"
export UTGID=${1};
cd /group/online/dataflow/cmtuser/Online/MyOnline/cmt;
. setup.x86_64-slc6-gcc48-opt.vars;

exec -a ${UTGID}  ${GAUDIONLINEROOT}/${CMTCONFIG}/Gaudi.exe \
    libGaudiOnline.so OnlineTask -msgsvc=LHCb::FmcMessageSvc \
    -tasktype=LHCb::Class1Task \
    -main=${GAUDIONLINEROOT}/options/Main.opts \
    -opt=${FARMCONFIGROOT}/options/MEPInjector.opts;
