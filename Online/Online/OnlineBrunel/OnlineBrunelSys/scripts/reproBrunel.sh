#!/bin/bash

export UTGID
export LOGFIFO
cd `dirname $0`/../cmt
#. setup.slc4_amd64_gcc34.vars
. setup.x86_64-slc5-gcc43-opt.vars
BASE=`cd ..;dirname $PWD`

export PYTHONPATH=/group/online/dataflow/options/${PARTITION}/RECONSTRUCTION:${BASE}/InstallArea/python:${PYTHONPATH}
export DATAINTERFACE=`python /group/online/dataflow/scripts/getDataInterface.py`
TASK_CLASS_TYPE=${2}
TASK_TYPE=${3}
export TAN_PORT=YES
export TAN_NODE=${DATAINTERFACE}
export DIM_DNS_NODE=${4}
export ONLINETASKS=/group/online/dataflow/templates
#
ulimit -d 2097152
ulimit -m 2097152
ulimit -v 2097152
#
echo "[INFO] +++ Starting BRUNEL ${UTGID} of class ${TASKCLASS} ${TASK_TYPE} with DNS:${DIM_DNS_NODE} Version:${BRUNELROOT}"
exec -a ${UTGID} \
    $GAUDIONLINEROOT/$CMTCONFIG/Gaudi.exe libGaudiOnline.so OnlineTask \
    -msgsvc=LHCb::FmcMessageSvc \
    -tasktype=LHCb::${TASK_CLASS_TYPE}Task \
    -main=$ONLINETASKS/options/Main.opts \
    -opt=command="\
import Gaudi,GaudiKernel.ProcessJobOptions;\
from Gaudi.Configuration import importOptions;\
GaudiKernel.ProcessJobOptions.printing_level=999;\
importOptions('../python/BrunelOnline2.py');"
