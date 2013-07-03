#!/bin/bash
#
export TAN_PORT=YES;
export TAN_NODE=${DATAINTERFACE};
echo [DEBUG] SUBFARM_OPTIONS: ${SUBFARM_OPTIONS};
exec -a ${UTGID} ${Class2_task} -opts=../options/${TASK_TYPE}.opts
