#!/bin/bash
export UTGID
export LOGFIFO
export PARTITION;
export NBOFSLAVES
#
cd `dirname $0`/../cmt
#
TASK_CLASS_TYPE=${2}
export TASK_TYPE=${3}
export DIM_DNS_NODE=${4}
#
export CMTCONFIG=x86_64-slc6-gcc48-dbg
export CMTCONFIG=x86_64-slc6-gcc48-opt
#
. setup.${CMTCONFIG}.vars
BASE=`cd ..;dirname $PWD`
#
export PYTHONPATH=/group/online/dataflow/options/${PARTITION}/MONITORING:/group/online/hlt/conditions/RunChangeHandler:${BASE}/InstallArea/python:${PYTHONPATH}
#
export DATAINTERFACE=`python /group/online/dataflow/scripts/getDataInterface.py`
export TAN_PORT=YES
export TAN_NODE=${DATAINTERFACE}
export ONLINETASKS=/group/online/dataflow/templates
export PARTITIONOPTS=/group/online/dataflow/options/${PARTITION}/${PARTITION}_Info.opts;
#
ulimit -d 2097152
ulimit -m 2097152
ulimit -v 2097152
#
# Normal running, nothing special
echo "[INFO] +++ Starting BRUNEL ${UTGID} of class ${TASKCLASS} ${TASK_TYPE} with DNS:${DIM_DNS_NODE} Version:${BRUNELROOT}"
export LD_PRELOAD=${CHECKPOINTINGROOT}/../../InstallArea/${CMTCONFIG}/lib/libCheckpointing.so
exec -a ${UTGID} `which GaudiCheckpoint.exe` libGaudiOnline.so OnlineTask \
    -msgsvc=LHCb::FmcMessageSvc \
    -tasktype=LHCb::${TASK_CLASS_TYPE}Task \
    -main=$ONLINETASKS/options/Main.opts \
    -opt=command="\
import Gaudi,GaudiKernel.ProcessJobOptions;\
from Gaudi.Configuration import importOptions;\
GaudiKernel.ProcessJobOptions.printing_level=999;\
importOptions('../python/BrunelEvDisp.py');"
