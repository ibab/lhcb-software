#!/bin/bash
. `dirname ${0}`/preamble.sh
#
#  Buffer Manager & Network name server
#
start_opts_task  MEPInit $OPTS/MEPInit.opts
start_tan_server;
#
#  Monitors:
#
start_mbmmon;
start_tanmon;
#
sleep 4
#
start_opts_task  Moore_0  $OPTS/ReadMBM.opts;
start_opts_task  Moore_1  $OPTS/ReadMBM.opts;
start_opts_task  Moore_2  $OPTS/ReadMBM.opts;
start_opts_task  Moore_3  $OPTS/ReadMBM.opts;
#start_opts_task  Moore_4  $OPTS/ReadMBM.opts;
#start_opts_task  Moore_5  $OPTS/ReadMBM.opts;
#start_opts_task  Moore_6  $OPTS/ReadMBM.opts;
#
export OUTPUT_FILE="file://mdfOutput.dat";
export OUTPUT_FILE="/dev/null";
start_opts_task  DiskWr   $OPTS/DiskWR.opts;
start_opts_task  Receiver $OPTS/MDFReceiver.opts;
#start_opts_task  EvtServ  $OPTS/EventServer.opts;
start_opts_task  EvtServ  $OPTS/EventRequestServer.opts;
#
sleep 4
#
export RECEIVER=${NODENAME}_Receiver;
start_opts_task  Sender1  $OPTS/MDFSender.opts;
start_opts_task  Sender2  $OPTS/MDFSender.opts;
#
export EVENT_SERVER=${NODENAME}_EvtServ;
start_opts_task  EvtCons1 $OPTS/NetworkConsumer.opts;
start_opts_task  EvtCons2 $OPTS/NetworkConsumer.opts;
#
if test ! -f ./mepData_0.dat;
then
    `dirname ${0}`/CreateMEP.sh
fi;
# start_mbmdump;
# start_meps;
tail -n 3 ${0}
