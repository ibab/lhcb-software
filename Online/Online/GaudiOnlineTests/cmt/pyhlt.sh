#!/bin/bash
echo "All args: $*"
. preamble.sh
#rm /dev/shm/bm_* /dev/shm/sem.bm_* /dev/shm/TAN* /dev/shm/sem.TAN*
#
start_py_task MepInit   "import GaudiOnlineTests;GaudiOnlineTests.runMepBuffer(partitionBuffers=True)"
#
start_tan_server;
#
sleep 6
#
#  Monitors:
#
start_mbmmon;
#start_tanmon;
#
start_py_task Moore_0     "import GaudiOnlineTests;GaudiOnlineTests.runMBMRead(partitionBuffers=True)"
start_py_task Moore_1     "import GaudiOnlineTests;GaudiOnlineTests.runMBMRead(partitionBuffers=True)"
start_py_task Moore_2     "import GaudiOnlineTests;GaudiOnlineTests.runMBMRead(partitionBuffers=True)"
start_py_task Moore_3     "import GaudiOnlineTests;GaudiOnlineTests.runMBMRead(partitionBuffers=True)"
#start_py_task Moore_4    "import GaudiOnlineTests;GaudiOnlineTests.runMBMRead(partitionBuffers=True)"
#start_py_task Moore_5    "import GaudiOnlineTests;GaudiOnlineTests.runMBMRead(partitionBuffers=True)"
#start_py_task Moore_6    "import GaudiOnlineTests;GaudiOnlineTests.runMBMRead(partitionBuffers=True)"
#start_py_task Moore_7    "import GaudiOnlineTests;GaudiOnlineTests.runMBMRead(partitionBuffers=True)"
#
sleep 3
start_py_task Receiver "import GaudiOnlineTests;GaudiOnlineTests.runReceiver(buffer='Overflow',partitionBuffers=True)"
start_py_task DiskWR   "import GaudiOnlineTests;GaudiOnlineTests.runDiskWR('Overflow',partitionBuffers=True,decode=False,output='/dev/null')"
start_py_task EvtServ   "import GaudiOnlineTests;GaudiOnlineTests.runEvtServer(buffer='Overflow',partitionBuffers=True)"
#
sleep 4
#
start_py_task Sender1 "import GaudiOnlineTests;GaudiOnlineTests.runSender('${NODENAME}_Receiver',partitionBuffers=True)"
start_py_task Sender2 "import GaudiOnlineTests;GaudiOnlineTests.runSender('${NODENAME}_Receiver',partitionBuffers=True)"
#
start_py_task EvtCons1 "import GaudiOnlineTests;GaudiOnlineTests.runNetCons('${NODENAME}_EvtServ',1,0.01)"
start_py_task EvtCons2 "import GaudiOnlineTests;GaudiOnlineTests.runNetCons('${NODENAME}_EvtServ',1,0.01)"
#
# Testing panormaix interface:
#start_gaudi_task Panoramix "import PanoramixSim;PanoramixSim.run('${NODENAME}/EvtServ')"
#
if test ! -f ./mepData_0.dat;
then
    `dirname ${0}`/CreateMEP.sh
fi;
#
# For debugging enable this and disable any other
# start_mbmdump;
# start_meps;
tail -n 3 ${0}
