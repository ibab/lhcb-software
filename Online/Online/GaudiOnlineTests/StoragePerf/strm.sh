#!/bin/bash
TARGET=`echo ${1} | tr a-z A-Z`;
shift;
NUM_TASK=${1};
if test -z "${NUM_TASK}"; then
  NUM_TASK=0;
fi;
. `pwd`/preamble.sh;
#
#  Buffer Manager & Network name server
#
start_opts_task  StrmMBM StrmMBM.opts
#
#  Monitors:
#
start_mbmmon -p=TEST;
#
sleep 4
#
for task in $(seq -w 00 ${NUM_TASK}); do
    start_opts_task  RCVFull_${task} RCVFull.opts;
done;
