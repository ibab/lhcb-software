#!/bin/bash
#
export DQ_TYPE=DataQuality;
if test "${DQ_TYPE}" = "DataQuality_Brunel"; then
  cd /group/online/dataflow/cmtuser/OnlineBrunel/OnlineBrunelSys/scripts;
  . ./runDQ.sh Brunel Class1 ${TASK_TYPE} ${DIM_DNS_NODE}
elif test "${DQ_TYPE}" = "DataQuality"; then
  cd ${DATAQUALITYROOT}/scripts;
  . ./runDataQuality.sh Brunel Class1 ${TASK_TYPE} ${DIM_DNS_NODE}
else
  export CHECKPOINT_SETUP_OPTIONS=${FARMCONFIGROOT}/options/Checkpoint.opts;
  export PYTHONPATH=${FARMCONFIGROOT}/job:${PYTHONPATH};
  eval `python ${FARMCONFIGROOT}/job/ConfigureCheckpoint.py -r ${RUNINFO} -s`;
  exec -a ${UTGID} ${Checkpoint_task} -opts=../options/${TASK_TYPE}.opts ${APP_STARTUP_OPTS};
fi;
