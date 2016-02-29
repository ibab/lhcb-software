#!/bin/bash
#
NODE=`hostname -s | tr a-z A-Z`;
export NODE_OPTIONS=${DYNAMIC_OPTS}/Nodes/${NODE}_TriggerInfo.opts;
export GO_SERVICE_NAME=${PARTITION_NAME}_${NODE}/Trigger;
##echo "[error] Go service name is: ${GO_SERVICE_NAME}";
exec -a ${UTGID} ${Class2_task} -opts=../options/${TASK_TYPE}.opts
