#!/bin/bash
# =========================================================================
#
#  Default script to start the passthrough process on a farm node.
#
#  Author   M.Frank
#  Version: 1.0
#  Date:    20/05/2013
#
# =========================================================================
#
export ONLINETASKS=/group/online/dataflow/templates
export PARTITIONOPTS=/group/online/dataflow/options/${PARTNAME}/${PARTNAME}_Info.opts
export CHECKPOINT_SETUP_OPTIONS=${FARMCONFIGROOT}/options/Checkpoint.opts;
export CHECKPOINT_RESTART_OPTIONS=${FARMCONFIGROOT}/options/CheckpointRestart.opts;

OPTIONS=-opts=../options/${TASK_TYPE}.opts;

if test "${TAE_PROCESSING}" = "TAE";
then
    OPTIONS=-opts=../options/${TASK_TYPE}_TAE.opts;
fi;
#
export PYTHONPATH=${FARMCONFIGROOT}/job:${PYTHONPATH};
echo exec -a ${UTGID} ${Checkpoint_task} ${OPTIONS};

#gdb --args  ${Checkpoint_task} -opts=../options/${TASK_TYPE}.opts -auto
##exit 1;
##Debug: python ${FARMCONFIGROOT}/job/ConfigureShell.py;

eval `python ${FARMCONFIGROOT}/job/ConfigureShell.py`;
if test "${MOORESTARTUP_MODE}" = "RESTORE";      ## RunInfo flag=2
    then
    ##Debug: python -c "import ConfigureFromCheckpoint";
    eval "python -c \"import ConfigureFromCheckpoint\" | ${RESTORE_CMD}";
else
    exec -a ${UTGID} ${Checkpoint_task} ${OPTIONS} ${APP_STARTUP_OPTS};
fi;
