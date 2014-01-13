#!/bin/bash
. ${GAUDIONLINEROOT}/tests/cmt/preamble.sh
#rm /dev/shm/bm_* /dev/shm/sem.bm_* /dev/shm/TAN* /dev/shm/sem.TAN*
export OPTS=$GAUDIONLINEROOT/options
#
#
start_py_task MepInit   "import GaudiOnlineTests;GaudiOnlineTests.runMepBuffer()"
start_py_task MbmOutput "import GaudiOnlineTests;GaudiOnlineTests.runOutBuffer()"
#
sleep 4
$BIGTERM MBMMon@${HOST}     -e "export UTGID=${NODENAME}/MBMMon;    exec -a \${UTGID} $gaudi_run libOnlineKernel.so mbm_mon"&
#
$MINITERM EvtProd@${HOST}   -e "export UTGID=${NODENAME}/EvtProd;   exec -a \${UTGID} $Class1_task -opt=$OPTS/MEPConverter.opts"&
start_py_task EvtHolder   "import GaudiOnlineTests;GaudiOnlineTests.runEvtHolder()"
start_py_task Moore_TMO   "import GaudiOnlineTests;GaudiOnlineTests.runMBMReadTimeout2()"
#start_py_task Moore_SEGV  "import GaudiOnlineTests;GaudiOnlineTests.runMBMReadSEGV()"
#start_py_task Moore_0     "import GaudiOnlineTests;GaudiOnlineTests.runMBMRead()"
#start_py_task Moore_1     "import GaudiOnlineTests;GaudiOnlineTests.runMBMRead()"
#
# export UTGID=${NODENAME}/prod_0; $gaudi_run libGaudiOnline.so mep_producer -n=prod_0 -p=333 -s=500 -r=2
tail -n 3 ${0}
