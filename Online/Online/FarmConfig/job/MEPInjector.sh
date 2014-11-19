#!/bin/bash
export UTGID=${1};
. /group/online/dataflow/scripts/preamble.sh
#
exec -a ${UTGID} GaudiOnlineExe.exe \
    libGaudiOnline.so OnlineTask -msgsvc=LHCb::FmcMessageSvc \
    -tasktype=LHCb::Class1Task \
    -main=${GAUDIONLINEROOT}/options/Main.opts \
    -opt=${FARMCONFIGROOT}/options/MEPInjector.opts;
