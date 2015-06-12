#!/bin/bash
#
export UTGID
export LOGFIFO
export PARTITION;
export NBOFSLAVES
#
cd `dirname $0`/../..;
##echo "[INFO] Working directory: `pwd`";
#
TASK_CLASS_TYPE=${2}
export TASK_TYPE=${3}
export DIM_DNS_NODE=${4}
#
export CMTCONFIG=x86_64-slc6-gcc48-opt;
#
. setup.${CMTCONFIG}.vars
#
export PYTHONPATH=${DATAQUALITYSYSROOT}/python:/group/online/dataflow/options/${PARTITION_NAME}:${PYTHONPATH}
#
ulimit -d 2097152
ulimit -m 2097152
ulimit -v 2097152
#
## Normal running, nothing special
echo "[INFO] +++ Starting DATAQUALITY ${UTGID} of class ${TASKCLASS} ${TASK_TYPE} with DNS:${DIM_DNS_NODE} Version:${BRUNELROOT}";
##
export DQ_DEBUG=YES;
export CHECKPOINTING_BIN=${CHECKPOINTINGROOT}/../../InstallArea/${CMTCONFIG};
export LD_PRELOAD=${CHECKPOINTING_BIN}/lib/libCheckpointing.so;
    exec -a ${UTGID} `which GaudiCheckpoint.exe` libGaudiOnline.so OnlineTask \
	-msgsvc=LHCb::FmcMessageSvc \
	-tasktype=LHCb::${TASK_CLASS_TYPE}Task \
	-main=$ONLINETASKS/options/Main.opts \
	-opt=command="import DQTask.DataQuality as task; task.run()"
##  -auto
