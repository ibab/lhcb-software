#!/bin/bash
#
. ./DQConfig.sh;
exec -a ${UTGID} ${Class2_task} -opts=../options/DQ/${TASK_TYPE}.opts;
