#!/bin/bash
#
#    Checked and revised for new buffer manager  17/01/2014 M.Frank
#
. `dirname ${0}`/preamble.sh
#
start_opts_task  MEPInit $OPTS/MEPInit.opts
start_gaudi_task TANServ libOnlineKernel.so tan_nameserver -a -tcp -d
start_tan_server;
#
#  Monitors:
#
start_mbmmon;
start_tanmon;
#
sleep 2
#
start_opts_task  Receiver $OPTS/MDFReceiver.opts;
#
start_opts_task TMA_0  $OPTS/ReadTAE.opts;
start_opts_task TMA_1  $OPTS/ReadTAE.opts;
start_opts_task TMA_2  $OPTS/ReadTAE.opts;
#
sleep 2
#
export RECEIVER=${NODENAME}_Receiver;
start_opts_task  Sender      $OPTS/MDFSender.opts;
export OUTPUT_FILE="file://taeOutput.dat";
export OUTPUT_FILE="/dev/null";
start_opts_task  Writer      $OPTS/DiskWR.opts;
#
if test ! -f ./taeData_0.dat;
then
    `dirname ${0}`/CreateMEP.sh -tae
fi;
# start_mbmdump;
# start_tae;
tail -n 3 ${0}
