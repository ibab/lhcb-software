#!/bin/bash

export UTGID
export LOGFIFO
export PARTITION;
#
cd `dirname $0`/../cmt
#
TASK_CLASS_TYPE=${2}
TASK_TYPE=${3}
export DIM_DNS_NODE=${4}
#
export CMTCONFIG=x86_64-slc5-gcc43-dbg
export CMTCONFIG=x86_64-slc5-gcc43-opt
#
. setup.${CMTCONFIG}.vars
BASE=`cd ..;dirname $PWD`

export PYTHONPATH=/group/online/dataflow/options/${PARTITION}/RECONSTRUCTION:${BASE}/InstallArea/python:${PYTHONPATH}
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
exec_restart="restart";
#
if test "${GAUDITASK_OPTS}" = "-checkpoint";
then
    export CHECKPOINT_FILE;
    export PYTHONPATH=${CHECKPOINTDIR}:${PYTHONPATH};
    export LD_PRELOAD=${CHECKPOINTINGROOT}/${CMTCONFIG}/libCheckpointing.so
    OBS=${ONLINEBRUNELSYSROOT};
    echo "[INFO] +++ Checkpoint BRUNEL GAUDITASK_OPTS:   ${GAUDITASK_OPTS} "
    echo "[INFO] +++ Checkpoint BRUNEL CHECKPOINT_FILE:  ${CHECKPOINT_FILE}"
    echo "[INFO] +++ Checkpoint BRUNEL ${UTGID} with DNS:${DIM_DNS_NODE} Checkpoint:${CHECKPOINT_FILE}";
    `which GaudiCheckpoint.exe` libGaudiOnline.so OnlineTask \
	-checkpoint \
	-msgsvc=LHCb::FmcMessageSvc \
	-tasktype=LHCb::Class1Task \
	-main=$GAUDIONLINEROOT/options/Main.opts \
	-opt=command="\
import Gaudi,GaudiKernel.ProcessJobOptions;\
from Gaudi.Configuration import importOptions;\
GaudiKernel.ProcessJobOptions.printing_level=999;\
importOptions('../python/BrunelOnline.py');"
	
elif test "${exec_restart}" = "restart" || test "${GAUDITASK_OPTS}" = "-restart";
then

    OBS=${ONLINEBRUNELSYSROOT};
    export CHECKPOINT_FILE;
    echo "[INFO] +++ Starting BRUNEL ${UTGID} with DNS:${DIM_DNS_NODE} Checkpoint:${CHECKPOINT_FILE}"
    unset LD_PRELOAD;
    unset LD_LIBRARY_PATH;
    `which python` ${OBS}/python/BrunelOnlineRestart.py | exec -a ${UTGID} `which restore.exe` -p 3 -e -i ${CHECKPOINT_FILE};

else

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
importOptions('../python/BrunelOnline.py');"

fi;
