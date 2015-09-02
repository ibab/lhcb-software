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
start_opts_task  RecvMBM RecvMBM.opts
#opts_task  RecvMBM RecvMBM.opts
#start_tan_server;
#
#  Monitors:
#
start_mbmmon;
#start_tanmon;
#
sleep 4
#
#start_opts_task  Moore_0  $OPTS/ReadMBM.opts;
#start_opts_task  Moore_1  $OPTS/ReadMBM.opts;
#start_opts_task  Receiver $OPTS/MDFReceiver.opts;
#
#sleep 4
#
#export RECEIVER=${TARGET}::${TARGET}_RCVFull_00;
for task in $(seq -w 00 ${NUM_TASK}); do
    export RECEIVER=${TARGET}-d1::${TARGET}_RCVFull_${task};
    start_opts_task SNDFull_${task}  SNDFull.opts;
done;
#
#GaudiOnlineExe.exe libGaudiOnline.so mdf_producer -n=prod_0 -p=333 -s=500 -b=Events_LHCb -f=
tail -n 3 ${0};
