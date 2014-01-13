#!/bin/bash
. `dirname ${0}`/preamble.sh
#
start_py_task MepInit   "import GaudiOnlineTests;GaudiOnlineTests.runMepBuffer()"
start_py_task MbmOutput "import GaudiOnlineTests;GaudiOnlineTests.runOutBuffer()"
#
$MINITERM TanServer@${HOST} -e "export UTGID=${NODENAME}/TANServer; exec -a \${UTGID} $gaudi_run libOnlineKernel.so tan_nameserver -a -tcp -d"&
#
sleep 6
#
#  Monitors:
#
$BIGTERM MBMMon@${HOST}     -e "export UTGID=${NODENAME}/MBMMon;    exec -a \${UTGID} $gaudi_run libOnlineKernel.so mbm_mon"&
#
$MINITERM EvtProd@${HOST}   -e "export UTGID=${NODENAME}/EvtProd;   exec -a \${UTGID} $Class1_task -opt=$OPTS/MEPConverter.opts"&
# start_py_task EvtProd   "import GaudiOnlineTests;GaudiOnlineTests.runEvtProd()"
start_py_task EvtHolder "import GaudiOnlineTests;GaudiOnlineTests.runEvtHolder()"
start_py_task Moore_0   "import GaudiOnlineTests;GaudiOnlineTests.runMBMRead()"
#
sleep 3
start_py_task Reformatter "import GaudiOnlineTests;GaudiOnlineTests.runReformatter(routing=0xAFFE)"
start_py_task DiskWR   "import GaudiOnlineTests;GaudiOnlineTests.runDiskWR('OUT',True,False,'./mdfOutput.dat')"
#
# $BIGTERM MBMDump@${HOST} -e "export UTGID=${NODENAME}/MBMDump; $gaudi_run libMBMDump.so mbmdump" &
# export UTGID=${NODENAME}/prod_0; $gaudi_run libGaudiOnline.so mep_producer -n=prod_0 -p=333 -s=500 -r=2
tail -n 3 ${0}
