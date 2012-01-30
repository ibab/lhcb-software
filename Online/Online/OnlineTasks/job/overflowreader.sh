#!/bin/bash
. ./pathsetupslc5_dbg
export DEBUGGING=1;
export PARTNAME=$2;
export LOGFIFO=/tmp/logGaudi.fifo
export ONLINETASKS=/group/online/dataflow/templates;
export INFOOPTIONS=/group/online/dataflow/options/${PARTNAME}/${PARTNAME}_Info.opts;
exec -a ${UTGID} ${GAUDIONLINEROOT}/${CMTCONFIG}/Gaudi.exe \
    ${GAUDIONLINEROOT}/${CMTCONFIG}/libGaudiOnline.so OnlineTask \
    -msgsvc=LHCb::FmcMessageSvc -tasktype=LHCb::Class2Task \
    -main=/group/online/dataflow/templates/options/Main.opts \
    -opts=${ONLINETASKSROOT}/hltopts/OverflowReader.opts
