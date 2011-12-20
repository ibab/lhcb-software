#!/bin/bash
echo "All args: $*"
. ${GAUDIONLINEROOT}/tests/cmt/preamble.sh
#rm /dev/shm/bm_* /dev/shm/sem.bm_* /dev/shm/TAN* /dev/shm/sem.TAN*
#
start_py_task MBMBuffers "import GaudiOnlineTests;GaudiOnlineTests.runHlt2Buffer()"
#
#$MINITERM TanServer@${HOST} -e "export UTGID=${NODENAME}/TANServer; exec -a \${UTGID} $gaudi_run libOnlineKernel.so tan_nameserver -a -tcp -d"&
#
sleep 6
#
#  Monitors:
#
$BIGTERM MBMMon@${HOST}     -e "export UTGID=${NODENAME}/MBMMon;    exec -a \${UTGID} $gaudi_run libOnlineKernel.so mbm_mon"&
#
start_py_task Hlt1_0     "import GaudiOnlineTests;GaudiOnlineTests.runHlt1Read()"
start_py_task Hlt1_1     "import GaudiOnlineTests;GaudiOnlineTests.runHlt1Read()"
start_py_task Hlt1_2     "import GaudiOnlineTests;GaudiOnlineTests.runHlt1Read()"
start_py_task Hlt1_3     "import GaudiOnlineTests;GaudiOnlineTests.runHlt1Read()"
#
start_py_task Hlt2_0     "import GaudiOnlineTests;GaudiOnlineTests.runHlt2Read()"
#start_py_task Hlt2_1     "import GaudiOnlineTests;GaudiOnlineTests.runHlt2Read()"
#start_py_task Hlt2_2     "import GaudiOnlineTests;GaudiOnlineTests.runHlt2Read()"
#
start_py_task Shuffle_0  "import GaudiOnlineTests;GaudiOnlineTests.runHltShuffle()"

# For debugging enable this and disable any other
#$MINITERM Sender@${HOST}    -e "export UTGID=${NODENAME}/DbgTask   ; cat gaudi.gdb; gdb -x gaudi.gdb $GAUDIONLINEROOT/$CMTCONFIG/Gaudi.exe" &
# $BIGTERM MBMDump@${HOST} -e "export UTGID=${NODENAME}/MBMDump; $gaudi_run libMBMDump.so mbmdump" &
# export UTGID=${NODENAME}/prod_0; $gaudi_run libGaudiOnline.so mep_producer -n=prod_0 -p=333 -s=500 -r=2
tail -n 3 ${0}
