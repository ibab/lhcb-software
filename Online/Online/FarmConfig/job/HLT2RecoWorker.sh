#!/bin/bash
#
. ./HLT2RecoConfig.sh;
##. ./EmptyJob.sh;
export CHECKPOINT_SETUP_OPTIONS=${FARMCONFIGROOT}/options/Checkpoint.opts;
export PYTHONPATH=${FARMCONFIGROOT}/job:${PYTHONPATH};
eval `python ${FARMCONFIGROOT}/job/ConfigureCheckpoint.py -r ${RUNINFO} -s`;
exec -a ${UTGID} ${Checkpoint_task} -opts=../options/${TASK_TYPE}.opts ${APP_STARTUP_OPTS};
