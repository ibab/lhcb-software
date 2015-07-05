#!/bin/bash
#
export UTGID
export LOGFIFO
export PARTITION;
export NBOFSLAVES
export ONLINETASKS=/group/online/dataflow/templates;
#
cd /group/online/dataflow/cmtuser/DataQuality;
##echo "[INFO] Working directory: `pwd`";
#
TASK_CLASS_TYPE=${2}
export TASK_TYPE=${3}
export DIM_DNS_NODE=${4}
#
export CMTCONFIG=x86_64-slc6-gcc48-opt;
#
. setup.${CMTCONFIG}.vars
##echo "[ERROR] Exec  setup.${CMTCONFIG}.vars";
#
ulimit -v 3221225472
#

if test -z "${RUNINFO}"; then
  export RUNINFO=/group/online/dataflow/options/${PARTITION_NAME}/OnlineEnvBase.py;
fi;
export PYTHONPATH=`dirname ${RUNINFO}`:$PYTHONPATH;
eval `python ${FARMCONFIGROOT}/job/ConfigureCheckpoint.py --runinfo ${RUNINFO} --brunel --environ --task_type=DataQuality`;
#
# Enable this for debugging only:
##python ${FARMCONFIGROOT}/job/ConfigureCheckpoint.py --runinfo ${RUNINFO} --brunel --environ --task_type=DataQuality;
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
    echo "[INFO] +++ Starting DataQuality ${UTGID} in RESTORE mode.";
    eval "python ${FARMCONFIGROOT}/job/ConfigureFromCheckpoint.py | ${RESTORE_CMD}";
    #
    #
else
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
	${APP_STARTUP_OPTS} -opt=command="import DQTask.DataQuality as task; task.run()"
    ##  -auto
fi;
