#!/bin/bash
TARGET=${1};
. `pwd`/preamble.sh;
#
#  Buffer Manager & Network name server
#
start_opts_task  StrmMBM StrmMBM.opts
#
#  Monitors:
#
start_mbmmon;
#
sleep 4
#
start_opts_task  RCVFull_00 RCVFull.opts;
start_opts_task  RCVFull_01 RCVFull.opts;
start_opts_task  RCVFull_02 RCVFull.opts;
start_opts_task  RCVFull_03 RCVFull.opts;
start_opts_task  RCVFull_04 RCVFull.opts;
