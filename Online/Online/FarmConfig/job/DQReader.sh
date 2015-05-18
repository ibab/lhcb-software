#!/bin/bash
#
. ./DQConfig.sh;
export NODE_OPTIONS=../options/Empty.opts;
exec -a ${UTGID} ${Class2_task} -opts=../options/DQ/${TASK_TYPE}.opts

