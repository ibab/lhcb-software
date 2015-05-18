#!/bin/bash
#
. ./DQConfig.sh;
exec -a ${UTGID} ${Class1_task} -opts=../options/DQ/${TASK_TYPE}.opts;
