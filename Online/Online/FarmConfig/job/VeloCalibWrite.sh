#!/bin/bash
#
export TAN_PORT=YES
export TAN_NODE=${DATAINTERFACE}
#
exec -a ${UTGID} ${Class2_task} -opts=../options/${TASK_TYPE}.opts
