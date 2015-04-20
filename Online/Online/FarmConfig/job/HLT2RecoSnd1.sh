#!/bin/bash
#
. ./HLT2RecoConfig.sh;
exec -a ${UTGID} ${Class2_task} -opts=../options/${TASK_TYPE}.opts;
