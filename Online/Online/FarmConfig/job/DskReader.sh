#!/bin/bash
#
NODE=`hostname -s | tr a-z A-Z`;
export NODE_OPTIONS=${DYNAMIC_OPTS}/Nodes/${NODE}_TriggerInfo.opts;
export NODE_OPTIONS=../options/Empty.opts;
#echo "[ERROR] NODE_OPTIONS: ${NODE_OPTIONS}" > /tmp/logGaudi.fifo;
#
exec -a ${UTGID} ${Class2_task} -opts=../options/${TASK_TYPE}.opts
