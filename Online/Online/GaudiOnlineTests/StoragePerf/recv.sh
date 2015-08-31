#!/bin/bash
TARGET=${1};
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
export RECEIVER=${TARGET}-d1::${TARGET}_RCVFull_00;
start_opts_task SNDFull_00  SNDFull.opts;
export RECEIVER=${TARGET}-d1::${TARGET}_RCVFull_01;
start_opts_task SNDFull_01  SNDFull.opts;
export RECEIVER=${TARGET}-d1::${TARGET}_RCVFull_02;
start_opts_task SNDFull_02  SNDFull.opts;
export RECEIVER=${TARGET}-d1::${TARGET}_RCVFull_03;
start_opts_task SNDFull_03  SNDFull.opts;
export RECEIVER=${TARGET}-d1::${TARGET}_RCVFull_04;
start_opts_task SNDFull_04  SNDFull.opts;
#
#GaudiOnlineExe.exe libGaudiOnline.so mdf_producer -n=prod_0 -p=333 -s=500 -b=Events_LHCb -f=
tail -n 3 ${0};
