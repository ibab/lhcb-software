#!/bin/bash
echo "All args: $*"
. ${GAUDIONLINEROOT}/tests/cmt/preamble.sh
#
start_py_task MBMBuffers "import GaudiOnlineTests;GaudiOnlineTests.runHlt2Buffer()"
#
sleep 4
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
#
start_py_task Shuffle_0  "import GaudiOnlineTests;GaudiOnlineTests.runHltShuffle()"

# For debugging enable this and disable any other
#$MINITERM Sender@${HOST}    -e "export UTGID=${NODENAME}/DbgTask   ; cat gaudi.gdb; gdb -x gaudi.gdb $GAUDIONLINEROOT/$CMTCONFIG/Gaudi.exe" &
# $BIGTERM MBMDump@${HOST} -e "export UTGID=${NODENAME}/MBMDump; $gaudi_run libMBMDump.so mbmdump" &
# export UTGID=${NODENAME}/prod_0; $gaudi_run libGaudiOnline.so mep2mbm_producer -n=prod_0 -p=333 -s=500 -r=2 -i=Mep -f=mepData_0.dat
tail -n 3 ${0}
