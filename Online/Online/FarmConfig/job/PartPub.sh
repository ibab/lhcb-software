#!/bin/bash
# =========================================================================
#
#  Default script to start the event reader task on the HLT farm
#
#  Author   M.Frank
#  Version: 1.0
#  Date:    20/05/2013
#
# =========================================================================
#
if [ "$PARTITION_NAME" = "LHCbA" ]; then
  cd ${FARMCONFIGROOT}/job
#  export LOGFIFO=/tmp/logSrv.fifo;
  exec -a ${UTGID} ${Class1_task} -opt=../options/DummyTask.opts
else
  exec -a ${UTGID} ${Class1_task} -opts=../options/pooper.opts
fi;
