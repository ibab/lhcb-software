#!/bin/bash
#
export DQ_TYPE=DataQuality;
if test "${DQ_TYPE}" = "DataQuality"; then
  cd /group/online/dataflow/cmtuser/OnlineBrunel/OnlineBrunelSys/scripts;
  . ./runDQ.sh Brunel Class1 ${TASK_TYPE} ${DIM_DNS_NODE}
else
  . ./DQConfig.sh;
  export CHECKPOINT_SETUP_OPTIONS=${FARMCONFIGROOT}/options/Checkpoint.opts;
  export PYTHONPATH=${FARMCONFIGROOT}/job:${PYTHONPATH};
  eval `python ${FARMCONFIGROOT}/job/ConfigureCheckpoint.py -r ${RUNINFO} -s`;
  exec -a ${UTGID} ${Checkpoint_task} -opts=../options/DQ/${TASK_TYPE}.opts ${APP_STARTUP_OPTS};
fi;
