#!/bin/bash
#
NODE=`hostname -s | tr a-z A-Z`;
export NODE_OPTIONS=${DYNAMIC_OPTS}/Nodes/${NODE}_TriggerInfo.opts;
exec -a ${UTGID} ${Class2_task} -opts=../options/${TASK_TYPE}.opts
