#!/bin/bash
. `dirname ${0}`/preamble.sh
#rm /dev/shm/bm_* /dev/shm/sem.bm_* /dev/shm/TAN* /dev/shm/sem.TAN*
#
#
$MINITERM MBMInit@${HOST}   -e "export UTGID=MEPInit;   exec -a \${UTGID} $gaudi_task -opts=$OPTS/MEPInit.opts"&
$MINITERM MBMOutput@${HOST} -e "export UTGID=MBMOutput; exec -a \${UTGID} $gaudi_task -opts=$OPTS/MBMinit.opts"&
$MINITERM TanServer@${HOST} -e "export UTGID=TANServer; exec -a \${UTGID} $test_exe tan_nameserver -a -tcp -d"&
#
sleep 6
#
#  Monitors:
#
$BIGTERM MBMMon@${HOST}     -e "export UTGID=MBMMon;    exec -a \${UTGID} $gaudi_run libOnlineKernel.so mbm_mon"&
$BIGTERM TANMon@${HOST}     -e "export UTGID=TANMon;    exec -a \${UTGID} $gaudi_run libOnlineKernel.so tanmon -c"&
#
$MINITERM EvtProd@${HOST}   -e "export UTGID=EvtProd;   exec -a \${UTGID} $gaudi_task -opt=$OPTS/MEPConverter.opts"&
$MINITERM EvtHolder@${HOST} -e "export UTGID=EvtHolder; exec -a \${UTGID} $gaudi_task -opt=$OPTS/MEPHolder.opts"&
$MINITERM Moore_0@${HOST}   -e "export UTGID=Moore_0;   exec -a \${UTGID} $gaudi_task -opt=$OPTS/ReadMBM.opts"&
$MINITERM Moore_1@${HOST}   -e "export UTGID=Moore_1;   exec -a \${UTGID} $gaudi_task -opt=$OPTS/ReadMBM.opts"&
$MINITERM Moore_2@${HOST}   -e "export UTGID=Moore_2;   exec -a \${UTGID} $gaudi_task -opt=$OPTS/ReadMBM.opts"&
$MINITERM Receiver@${HOST}  -e "export UTGID=Receiver;  exec -a \${UTGID} $gaudi_task -opt=$OPTS/MDFReceiver.opts"&
# $MINITERM DiskWR@${HOST}  -e "export UTGID=DiskWR;    exec -a \${UTGID} $gaudi_task -opt=$OPTS/DiskWR.opts"&
$MINITERM EvtServ@${HOST}   -e "export UTGID=EvtServ;   exec -a \${UTGID} $gaudi_task -opt=$OPTS/EventServer.opts"&
#
sleep 2
#
$MINITERM Sender@${HOST}    -e "export UTGID=Sender;    exec -a \${UTGID} $gaudi_task -opt=$OPTS/MDFSender.opts"&
$MINITERM Sender2@${HOST}   -e "export UTGID=Sender2;   exec -a \${UTGID} $gaudi_task -opt=$OPTS/MDFSender.opts"&
#
$MINITERM EvtCons1@${HOST}  -e "export UTGID=EvtCons1;  exec -a \${UTGID} $gaudi_task -opt=$OPTS/NetworkConsumer.opts"&
$MINITERM EvtCons2@${HOST}  -e "export UTGID=EvtCons2;  exec -a \${UTGID} $gaudi_task -opt=$OPTS/NetworkConsumer.opts"&
#
# For debugging enable this and disable any other
#$MINITERM Sender@${HOST}    -e "export UTGID=DbgTask   ; cat gaudi.gdb; gdb -x gaudi.gdb $GAUDIONLINEROOT/$CMTCONFIG/Gaudi.exe" &
# $BIGTERM MBMDump@${HOST} -e "export UTGID=MBMDump; $gaudi_run libMBMDump.so mbmdump" &
# export UTGID=prod_0; $gaudi_run libGaudiOnline.so mep_producer -n=prod_0 -p=333 -s=500 -r=2
tail -n 3 ${0}
