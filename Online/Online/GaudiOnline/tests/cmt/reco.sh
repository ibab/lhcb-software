#!/bin/bash
. `dirname ${0}`/preamble.sh
#rm /dev/shm/bm_* /dev/shm/sem.bm_* /dev/shm/TAN* /dev/shm/sem.TAN*
export NODENAME=`python -c "print '$HOST'.split('.')[0]"`
export OPTS=$GAUDIONLINEROOT/recopts
#
#
$MINITERM RECMBM@${HOST}   -e "export UTGID=${NODENAME}/RECMBM;   exec -a \${UTGID} $gaudi_task -opts=$OPTS/RECBuffers.opts"&
$MINITERM TanServer@${HOST} -e "export UTGID=${NODENAME}/TANServer; exec -a \${UTGID} $gaudi_run libOnlineKernel.so tan_nameserver -a -tcp -d"&
#
sleep 4
#
#  Monitors:
#
$BIGTERM MBMMon@${HOST}     -e "export UTGID=${NODENAME}/MBMMon;    exec -a \${UTGID} $gaudi_run libOnlineKernel.so mbm_mon"&
$BIGTERM TANMon@${HOST}     -e "export UTGID=${NODENAME}/TANMon;    exec -a \${UTGID} $gaudi_run libOnlineKernel.so tanmon -c"&
#
#   Reconstruction node tasks
#
$MINITERM Brunel_0@${HOST}   -e "export UTGID=${NODENAME}/Brunel_0;   exec -a \${UTGID} $gaudi_task -opt=$OPTS/RECRead.opts"&
$MINITERM Brunel_1@${HOST}   -e "export UTGID=${NODENAME}/Brunel_1;   exec -a \${UTGID} $gaudi_task -opt=$OPTS/RECRead.opts"&
$MINITERM Brunel_2@${HOST}   -e "export UTGID=${NODENAME}/Brunel_2;   exec -a \${UTGID} $gaudi_task -opt=$OPTS/RECRead.opts"&
$MINITERM RECReceiver@${HOST}  -e "export UTGID=${NODENAME}/RECReceiver;  exec -a \${UTGID} $gaudi_task -opt=$OPTS/RECReceiver.opts"&
#
#   Output node tasks
#
$MINITERM OUTReceiver@${HOST}  -e "export UTGID=${NODENAME}/OUTReceiver;  exec -a \${UTGID} $gaudi_task -opt=$OPTS/OUTReceiver.opts"&
#
#
sleep 4
$MINITERM RECSender@${HOST}    -e "export UTGID=${NODENAME}/RECSender;    exec -a \${UTGID} $gaudi_task -opt=$OPTS/RECSender.opts"&
#
#   Input node tasks
#
$MINITERM INPUTSender@${HOST}    -e "export UTGID=${NODENAME}/INPUTSender;    exec -a \${UTGID} $gaudi_task -opt=$OPTS/INPUTSender.opts"&
#
#
$MINITERM RECWriter@${HOST}  -e "export UTGID=${NODENAME}/RECWriter;    exec -a \${UTGID} $gaudi_task -opt=$OPTS/RECWriter.opts"&
#
# For debugging enable this and disable any other
# $BIGTERM MBMDump@${HOST} -e "export UTGID=${NODENAME}/MBMDump; $gaudi_run libMBMDump.so mbmdump" &
# export UTGID=${NODENAME}/prod_0; $gaudi_run libGaudiOnline.so mep_producer -n=prod_0 -p=333 -s=500 -r=2
# $MINITERM MDF2MBM@${HOST}  -e "export UTGID=${NODENAME}/MDF2MBM;    exec -a \${UTGID} $gaudi_task -opt=../../recopts/DimRPCFileReader.opts"&
# $MINITERM Reader@${HOST}  -e "export UTGID=Reader;    exec -a \${UTGID} $gaudi_task -opt=../recopts/DimRPCFileReader.opts"&
tail -n 5 reco.sh | head -n 4
