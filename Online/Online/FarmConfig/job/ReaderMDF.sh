#!/bin/bash
#
NODE=`hostname -s | tr a-z A-Z`;
export NODE_OPTIONS=${DYNAMIC_OPTS}/Nodes/${NODE}_TriggerInfo.opts;
export GO_SERVICE_NAME=${PARTITION_NAME}_${NODE}/Trigger;
#
exec -a ${UTGID} ${Class2_task} -opts=../options/${TASK_TYPE}.opts
