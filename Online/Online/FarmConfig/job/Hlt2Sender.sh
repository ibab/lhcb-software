#!/bin/bash
#
export TAN_PORT=YES;
export TAN_NODE=${DATAINTERFACE};
echo [DEBUG] SUBFARM_OPTIONS: ${SUBFARM_OPTIONS};
###exec -a ${UTGID} ${Class2_task} -opts=../options/${TASK_TYPE}.opts
exec -a ${UTGID} $gaudi_task -tasktype=LHCb::Class2Task -main=../options/Hlt2Main.opts -opts=../options/${TASK_TYPE}.opts
