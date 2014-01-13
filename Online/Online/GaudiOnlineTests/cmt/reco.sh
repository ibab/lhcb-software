#!/bin/bash
. `dirname ${0}`/preamble.sh
#rm /dev/shm/bm_* /dev/shm/sem.bm_* /dev/shm/TAN* /dev/shm/sem.TAN*
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
#$MINITERM RECWriter@${HOST}  -e "export UTGID=${NODENAME}/RECWriter;    exec -a \${UTGID} $gaudi_task -opt=$OPTS/RecWriterNet.opts"&
#
# For debugging enable this and disable any other
# $BIGTERM MBMDump@${HOST} -e "export UTGID=${NODENAME}/MBMDump; $gaudi_run libMBMDump.so mbmdump" &
# start_py_task Mdf2Mbm "import GaudiOnlineTests;GaudiOnlineTests.runMDF2MBMRepro(['READ'],fname='root:/afs/cern.ch/user/f/frankb/cmtuser/data/mdfData.dat',partitionBuffers=False)"
tail -n 3 ${0}
