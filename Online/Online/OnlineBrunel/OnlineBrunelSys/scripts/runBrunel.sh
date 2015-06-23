#!/bin/bash
#
export UTGID
export LOGFIFO
export PARTITION;
export NBOFSLAVES
#
cd `dirname $0`/../..
##echo "[INFO] Working directory: `pwd`";
#
TASK_CLASS_TYPE=${2}
export TASK_TYPE=${3}
export DIM_DNS_NODE=${4}
#
if test "${PARTITION}" = "FEST"; then
    export CMTCONFIG=x86_64-slc6-gcc48-dbg;
elif test "${PARTITION}" = "LHCb"; then
    export CMTCONFIG=x86_64-slc6-gcc48-opt;
else
    export CMTCONFIG=x86_64-slc6-gcc48-opt;
fi
    export CMTCONFIG=x86_64-slc6-gcc48-opt;
#
##echo "Setup: `pwd`/setup.${CMTCONFIG}.vars";
. setup.${CMTCONFIG}.vars;
##echo $LD_LIBRARY_PATH;
#
export PYTHONPATH=/group/online/dataflow/options/${PARTITION}/RECONSTRUCTION:/group/online/hlt/conditions/RunChangeHandler:${PYTHONPATH}
#
export DATAINTERFACE=`python /group/online/dataflow/scripts/getDataInterface.py`
export TAN_PORT=YES
export TAN_NODE=${DATAINTERFACE}
export ONLINETASKS=/group/online/dataflow/templates
export PARTITIONOPTS=/group/online/dataflow/options/${PARTITION}/${PARTITION}_Info.opts;
#
ulimit -d 4545728
ulimit -m 4545728
ulimit -v 4545728
#
exec_restart="restart";
exec_restart="normal";
#
if test -z "${RUNINFO}"; then
  export RUNINFO=/group/online/dataflow/options/${PARTITION}/RECONSTRUCTION/OnlineEnvBase.py;
fi;
eval `python ${FARMCONFIGROOT}/job/ConfigureCheckpoint.py --runinfo ${RUNINFO} --brunel --environ`;
#
# Enable this for debugging only:
 python ${FARMCONFIGROOT}/job/ConfigureCheckpoint.py --runinfo ${RUNINFO} --brunel --environ;
#
#
if test "${exec_restart}" = "restart" -o "${APP_STARTUP_OPTS}" = "-restore";
then
    export LOCAL_CHECKPOINT_DIR=/scratchlocal/checkpoints;
    #
    # Enable this for debugging only:
    ##python       ${FARMCONFIGROOT}/job/ConfigureCheckpoint.py  --runinfo ${RUNINFO} --copy --libs --start --brunel;
    #
    # Copy checkpoint and extract libraries
    #
    eval `python ${FARMCONFIGROOT}/job/ConfigureCheckpoint.py  --runinfo ${RUNINFO} --copy --libs --start --brunel`;
    #
    # Start Brunel from checkpoint file
    #
    #python ${FARMCONFIGROOT}/job/ConfigureFromCheckpoint.py;
    echo "[INFO] Restore command: ${RESTORE_CMD}";
    echo "[INFO] +++ Starting BRUNEL ${UTGID} in RESTORE mode.";
    eval "python ${FARMCONFIGROOT}/job/ConfigureFromCheckpoint.py | ${RESTORE_CMD}";
    #
    #
else    #   Normal execution mode:
    #
    # Normal running, nothing special
    echo "[INFO] +++ Starting BRUNEL ${UTGID} of class ${TASKCLASS} ${TASK_TYPE} with DNS:${DIM_DNS_NODE} Version:${BRUNELROOT}";
    export LD_PRELOAD=${CHECKPOINTING_BIN}/lib/libCheckpointing.so;
    exec -a ${UTGID} `which GaudiCheckpoint.exe` libGaudiOnline.so OnlineTask \
	-msgsvc=LHCb::FmcMessageSvc \
	-tasktype=LHCb::${TASK_CLASS_TYPE}Task \
	-main=$ONLINETASKS/options/Main.opts \
	${APP_STARTUP_OPTS} -opt=command="\
import Gaudi,GaudiKernel.ProcessJobOptions;\
from Gaudi.Configuration import importOptions;\
GaudiKernel.ProcessJobOptions.printing_level=999;\
importOptions('OnlineBrunelSys/python/BrunelOnline.py');";
fi;
