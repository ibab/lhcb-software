#!/bin/bash
##echo "Error: Running passthrough mode $PARTNAME 1:$1 2:$2 3:$3 4:$4"
. ./pathsetupslc5_dbg
export DEBUGGING=1;
export PARTNAME=$3;
export LOGFIFO=/tmp/logGaudi.fifo
export ONLINETASKS=/group/online/dataflow/templates;
export INFOOPTIONS=/group/online/dataflow/options/${PARTNAME}/${PARTNAME}_Info.opts;
export NBOFSLAVES=$4;
if test -f /home/online/ONLINE/Online_${ONLINE_VERSION}/InstallArea/${CMTCONFIG}/lib/libCheckpointing.so;
    then
    export LD_PRELOAD=/home/online/ONLINE/Online_${ONLINE_VERSION}/InstallArea/${CMTCONFIG}/lib/libCheckpointing.so;
else
    export LD_PRELOAD=/sw/lib/lhcb/ONLINE/ONLINE_${ONLINE_VERSION}/InstallArea/${CMTCONFIG}/lib/libCheckpointing.so;
fi;
#
#
export PYTHONPATH=/group/online/dataflow/options/${PARTNAME}/HLT:$PYTHONPATH;
TAE_OPT=`python <</EOF
import OnlineEnvBase as O
if O.TAE>0: print '_TAE'
/EOF`
#
##echo "[Error] running options PassThrough${TAE_OPT}.opts"
#
exec -a ${UTGID} ${GAUDIONLINEROOT}/${CMTCONFIG}/GaudiCheckpoint.exe \
    ${GAUDIONLINEROOT}/${CMTCONFIG}/libGaudiOnline.so OnlineTask \
    -msgsvc=LHCb::FmcMessageSvc -tasktype=LHCb::Class1Task \
    -main=/group/online/dataflow/templates/options/Main.opts \
    -opts=${ONLINETASKSROOT}/hltopts/PassThrough${TAE_OPT}.opts
#
#
#
