#!/bin/bash
. `dirname ${0}`/preamble.sh
#rm /dev/shm/bm_* /dev/shm/sem.bm_* /dev/shm/TAN* /dev/shm/sem.TAN*
export NODENAME=`python -c "print '$HOST'.split('.')[0]"`
#
$MINITERM TAEinit@${HOST}   -e "export UTGID=${NODENAME}/TAEBuffers;exec -a \${UTGID} $gaudi_task -opts=$OPTS/TAEBuffers.opts"&
$MINITERM MBMInit@${HOST}   -e "export UTGID=${NODENAME}/MBMInit;   exec -a \${UTGID} $gaudi_task -opts=$OPTS/MBMinit.opts"&
$MINITERM TanServer@${HOST} -e "export UTGID=${NODENAME}/TANServer; exec -a \${UTGID} $gaudi_run libOnlineKernel.so tan_nameserver -a -tcp -d"&
#
sleep 4
#
#  Monitors:
$BIGTERM MBMMon@${HOST}     -e "export UTGID=${NODENAME}/MBMMon;  exec -a \${UTGID} $gaudi_run libOnlineKernel.so mbm_mon"&
$BIGTERM TANMon@${HOST}     -e "export UTGID=${NODENAME}/TANMon;  exec -a \${UTGID} $gaudi_run libOnlineKernel.so tanmon -c"&
$MINITERM Receiver@${HOST}  -e "export UTGID=${NODENAME}/Receiver;exec -a \${UTGID} $gaudi_task -opt=$OPTS/MDFReceiver.opts"&
sleep 2
#
$MINITERM TMA_0@${HOST}     -e "export UTGID=${NODENAME}/TMA_0;   exec -a \${UTGID} $gaudi_task -opt=$OPTS/ReadTAE.opts"&
$MINITERM TMA_1@${HOST}     -e "export UTGID=${NODENAME}/TMA_1;   exec -a \${UTGID} $gaudi_task -opt=$OPTS/ReadTAE.opts"&
$MINITERM TMA_2@${HOST}     -e "export UTGID=${NODENAME}/TMA_2;   exec -a \${UTGID} $gaudi_task -opt=$OPTS/ReadTAE.opts"&
$MINITERM Sender@${HOST}    -e "export UTGID=${NODENAME}/Sender;  exec -a \${UTGID} $gaudi_task -opt=$OPTS/TAESender.opts"&
$MINITERM TAEWriter@${HOST} -e "export UTGID=${NODENAME}/TAEWriter; exec -a \${UTGID} $gaudi_task -opt=$OPTS/TAEWriter.opts"&
#export UTGID=TMA_$$
#$gaudi_task -opt=$OPTS/TAESender.opts &
#
sleep 2
#
# $BIGTERM MBMDump@${HOST} -e "export UTGID=MBMDump; $gaudi_run libMBMDump.so mbmdump" &
# export UTGID=${NODENAME}/prod_0; $gaudi_run libGaudiOnline.so mep_producer -n=prod_0 -p=333 -s=500 -r=2
tail -n 3 ${0}
