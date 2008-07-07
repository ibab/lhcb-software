#!/bin/bash
. `dirname ${0}`/preamble.sh
#rm /dev/shm/bm_* /dev/shm/sem.bm_* /dev/shm/TAN* /dev/shm/sem.TAN*
export NODENAME=`python -c "print '$HOST'.split('.')[0]"`
#
#
$MINITERM MBMInit@${HOST}   -e "export UTGID=${NODENAME}/MEPInit;   exec -a \${UTGID} $Class0_task -opts=$OPTS/MEPInit.opts"&
$MINITERM MBMOutput@${HOST} -e "export UTGID=${NODENAME}/MBMOutput; exec -a \${UTGID} $Class0_task -opts=$OPTS/MBMinit.opts"&
$MINITERM TanServer@${HOST} -e "export UTGID=${NODENAME}/TANServer; exec -a \${UTGID} $gaudi_run libOnlineKernel.so tan_nameserver -a -tcp -d"&
#$MINITERM ROCollect@${HOST} -e "export UTGID=${NODENAME}/ROCollect; exec -a \${UTGID} $gaudi_run libGaudiOnline.so romon_collect -gbl=ROMonitor -size=64 -delay=500 -verbose"&
#$MINITERM ROPublish@${HOST} -e "export UTGID=${NODENAME}/ROPublish; exec -a \${UTGID} $gaudi_run libGaudiOnline.so romon_dimserver -gbl=ROMonitor -size=64 -delay=500 -verbose"&
#
sleep 4
#
#  Monitors:
#
$BIGTERM MBMMon@${HOST}     -e "export UTGID=${NODENAME}/MBMMon;    exec -a \${UTGID} $gaudi_run libOnlineKernel.so mbm_mon"&
$BIGTERM TANMon@${HOST}     -e "export UTGID=${NODENAME}/TANMon;    exec -a \${UTGID} $gaudi_run libOnlineKernel.so tanmon -c"&
#
$MINITERM EvtProd@${HOST}   -e "export UTGID=${NODENAME}/EvtProd;   exec -a \${UTGID} $Class1_task -opt=$OPTS/MEPConverter.opts"&
$MINITERM EvtHolder@${HOST} -e "export UTGID=${NODENAME}/EvtHolder; exec -a \${UTGID} $Class1_task -opt=$OPTS/MEPHolder.opts"&
$MINITERM Moore_0@${HOST}   -e "export UTGID=${NODENAME}/Moore_0;   exec -a \${UTGID} $Class1_task -opt=$OPTS/ReadMBM.opts"&
$MINITERM Moore_1@${HOST}   -e "export UTGID=${NODENAME}/Moore_1;   exec -a \${UTGID} $Class1_task -opt=$OPTS/ReadMBM.opts"&
$MINITERM Moore_2@${HOST}   -e "export UTGID=${NODENAME}/Moore_2;   exec -a \${UTGID} $Class1_task -opt=$OPTS/ReadMBM.opts"&
$MINITERM Moore_3@${HOST}   -e "export UTGID=${NODENAME}/Moore_3;   exec -a \${UTGID} $Class1_task -opt=$OPTS/ReadMBM.opts"&
#$MINITERM Moore_4@${HOST}   -e "export UTGID=${NODENAME}/Moore_4;   exec -a \${UTGID} $Class1_task -opt=$OPTS/ReadMBM.opts"&
#$MINITERM Moore_5@${HOST}   -e "export UTGID=${NODENAME}/Moore_5;   exec -a \${UTGID} $Class1_task -opt=$OPTS/ReadMBM.opts"&
#$MINITERM Moore_6@${HOST}   -e "export UTGID=${NODENAME}/Moore_6;   exec -a \${UTGID} $Class1_task -opt=$OPTS/ReadMBM.opts"&
#$MINITERM Moore_7@${HOST}   -e "export UTGID=${NODENAME}/Moore_7;   exec -a \${UTGID} $Class1_task -opt=$OPTS/ReadMBM.opts"&
$MINITERM Receiver@${HOST}  -e "export UTGID=${NODENAME}/Receiver;  exec -a \${UTGID} $Class1_task -opt=$OPTS/MDFReceiver.opts"&
###$MINITERM DiskWR@${HOST}  -e "export UTGID=${NODENAME}/DiskWR;    exec -a \${UTGID} $Class2_task -opt=$OPTS/DiskWR.opts"&
$MINITERM EvtServ@${HOST}   -e "export UTGID=${NODENAME}/EvtServ;   exec -a \${UTGID} $Class1_task -opt=$OPTS/EventServer.opts"&
#$MINITERM EvtServ@${HOST}   -e "export UTGID=${NODENAME}/EvtServ;   exec -a \${UTGID} $Class1_task -opt=$OPTS/EventRequestServer.opts"&
#
sleep 4
#
$MINITERM Sender@${HOST}    -e "export UTGID=${NODENAME}/Sender;    exec -a \${UTGID} $Class2_task -opt=$OPTS/MDFSender.opts"&
$MINITERM Sender2@${HOST}   -e "export UTGID=${NODENAME}/Sender2;   exec -a \${UTGID} $Class2_task -opt=$OPTS/MDFSender.opts"&
#
$MINITERM EvtCons1@${HOST}  -e "export UTGID=${NODENAME}/EvtCons1;  exec -a \${UTGID} $Class2_task -opt=$OPTS/NetworkConsumer.opts"&
$MINITERM EvtCons2@${HOST}  -e "export UTGID=${NODENAME}/EvtCons2;  exec -a \${UTGID} $Class2_task -opt=$OPTS/NetworkConsumer.opts"&
#
# For debugging enable this and disable any other
# $MINITERM Sender@${HOST}    -e "export UTGID=${NODENAME}/DbgTask   ; cat gaudi.gdb; gdb -x gaudi.gdb $GAUDIONLINEROOT/$CMTCONFIG/Gaudi.exe" &
# $BIGTERM MBMDump@${HOST} -e "export UTGID=${NODENAME}/MBMDump; $gaudi_run libMBMDump.so mbmdump" &
# export UTGID=${NODENAME}/prod_0; $gaudi_run libGaudiOnline.so mep_producer -n=prod_0 -p=333 -s=500 -r=2
tail -n 3 ${0}
