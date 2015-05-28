#!/bin/bash
#
. ./DQConfig.sh;
export NODE_OPTIONS=../options/Empty.opts;
export NODE_OPTIONS=/tmp/DATAQUALITY_OPT/ReaderInput.opts;
exec -a ${UTGID} ${Class2_task} -opts=../options/DQ/${TASK_TYPE}.opts

