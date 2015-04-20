#!/bin/bash
#
. ./HLT2RecoConfig.sh;
export NODE_OPTIONS=../options/Empty.opts;
exec -a ${UTGID} ${Class2_task} -opts=../options/${TASK_TYPE}.opts

