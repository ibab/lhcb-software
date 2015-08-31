#!/bin/bash
killall GaudiOnlineExe.exe;
. `pwd`/../cmt/preamble.sh NO_KILL;
export HOST=`echo $HOST | tr a-z A-Z`;
export PREAMBLE_OPTS=Preamble.opts;
export DATAFLOWDIR=/group/online/dataflow;
export DATAINTERFACE=`python ${DATAFLOWDIR}/scripts/getDataInterface.py`
export TAN_PORT=YES
export TAN_NODE=${DATAINTERFACE}
export ONLINETASKS=${DATAFLOWDIR}/templates;
export LOGFIFO=/tmp/logGaudi.fifo;
export msg_svc=LHCb::FmcMessageSvc;
export Class1_task="${GaudiOnlineExe} libGaudiOnline.so OnlineTask -tasktype=LHCb::Class1Task -msgsvc=$msg_svc -auto -main=$OPTS/Main.opts "
sleep 3;
#start_tan_server;
#start_tanmon;
#sleep 5;