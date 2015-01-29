#!/bin/bash

export UTGID
export LOGFIFO
export PARTITION;
#
cd `dirname $0`/../..;
#
TASK_CLASS_TYPE=${2}
export TASK_TYPE=${3}
export DIM_DNS_NODE=${4}
#
export CMTCONFIG=x86_64-slc6-gcc48-opt
export CMTCONFIG=x86_64-slc6-gcc48-dbg
#
. setup.${CMTCONFIG}.vars
export PYTHONPATH=/group/online/dataflow/options/${PARTITION}/RECONSTRUCTION:/group/online/hlt/conditions/RunChangeHandler:${BASE}/InstallArea/python:${PYTHONPATH}
#
#
export ONLINETASKS=/group/online/dataflow/templates
export PARTITIONOPTS=/group/online/dataflow/options/${PARTITION}/${PARTITION}_Info.opts;
#
ulimit -d 2097152
ulimit -m 2097152
ulimit -v 2097152
#
echo "[INFO] +++ Starting BRUNEL ${UTGID} of class ${TASKCLASS} ${TASK_TYPE} with DNS:${DIM_DNS_NODE}";
cd WebGLDisplaySys
exec -a ${UTGID} `which GaudiOnlineExe.exe` libGaudiOnline.so OnlineTask \
    -msgsvc=LHCb::FmcMessageSvc \
    -tasktype=LHCb::${TASK_CLASS_TYPE}Task \
    -main=$ONLINETASKS/options/Main.opts \
    -opt=command="\
import Gaudi,GaudiKernel.ProcessJobOptions;\
from Gaudi.Configuration import importOptions;\
GaudiKernel.ProcessJobOptions.printing_level=999;\
importOptions('./python/WebGLDisplay.py');"
