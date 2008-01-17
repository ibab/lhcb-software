#!/bin/bash
. `dirname ${0}`/preamble.sh
rm /dev/shm/bm_* /dev/shm/sem.bm_* /dev/shm/TAN* /dev/shm/sem.TAN*
#
$MINITERM TAEinit@${HOST}   -e "export UTGID=TAEBuffers;exec -a \${UTGID} $gaudi_task -opts=$OPTS/TAEBuffers.opts"&
$MINITERM MBMOutput@${HOST} -e "export UTGID=MBMOutput; exec -a \${UTGID} $gaudi_task -opts=$OPTS/MBMinit.opts"&
$MINITERM TanServer@${HOST} -e "export UTGID=TANServer; exec -a \${UTGID} $gaudi_run libOnlineKernel.so tan_nameserver -a -tcp -d"&
#
sleep 4
#
#  Monitors:
$BIGTERM MBMMon@${HOST}     -e "export UTGID=MBMMon;  exec -a \${UTGID} $gaudi_run libOnlineKernel.so mbm_mon"&
$BIGTERM TANMon@${HOST}     -e "export UTGID=TANMon;  exec -a \${UTGID} $gaudi_run libOnlineKernel.so tanmon -c"&
$MINITERM Receiver@${HOST}  -e "export UTGID=RECEIVER;exec -a \${UTGID} $gaudi_task -opt=$OPTS/MDFReceiver.opts"&
sleep 2
#
$MINITERM TMA_0@${HOST}     -e "export UTGID=TMA_0;   exec -a \${UTGID} $gaudi_task -opt=$OPTS/ReadTAE.opts"&
$MINITERM TMA_1@${HOST}     -e "export UTGID=TMA_1;   exec -a \${UTGID} $gaudi_task -opt=$OPTS/ReadTAE.opts"&
$MINITERM TMA_2@${HOST}     -e "export UTGID=TMA_2;   exec -a \${UTGID} $gaudi_task -opt=$OPTS/ReadTAE.opts"&
$MINITERM Sender@${HOST}    -e "export UTGID=Sender;  exec -a \${UTGID} $gaudi_task -opt=$OPTS/TAESender.opts"&
#$MINITERM DiskWR@${HOST}   -e "export UTGID=DiskWR;  exec -a \${UTGID} $gaudi_task -opt=$OPTS/DiskWR.opts"&
#export UTGID=TMA_$$
#$gaudi_task -opt=$OPTS/TAESender.opts &
#
sleep 2
#
# $BIGTERM MBMDump@${HOST} -e "export UTGID=MBMDump; $gaudi_run libMBMDump.so mbmdump" &
# export UTGID=prod_0; $gaudi_run libGaudiOnline.so mep_producer -n=prod_0 -p=333 -s=500 -r=2
tail -n 3 ${0}
