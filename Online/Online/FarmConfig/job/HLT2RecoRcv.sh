#!/bin/bash
#
. ./HLT2RecoConfig.sh;
exec -a ${UTGID} ${Class1_task} -opts=../options/${TASK_TYPE}.opts;
