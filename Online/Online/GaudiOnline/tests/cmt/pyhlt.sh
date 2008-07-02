#!/bin/bash
. `dirname ${0}`/preamble.sh
#rm /dev/shm/bm_* /dev/shm/sem.bm_* /dev/shm/TAN* /dev/shm/sem.TAN*
export NODENAME=`python -c "print '$HOST'.split('.')[0]"`
if test -z "${PYOPTS}";
  then
    export PYOPTS=${GAUDIONLINEROOT}/python
    export PYTHONPATH=${PYOPTS}:${PYTHONPATH}
fi;
#
start_task()
{
  $MINITERM ${1}@${HOST}   -e "export UTGID=${NODENAME}/${1};   exec -a \${UTGID} $gaudi_task -opt=command=\"${2}\""&
}
#
start_task MepInit   "import GaudiOnlineTests;GaudiOnlineTests.runMepBuffer()"
start_task MbmOutput "import GaudiOnlineTests;GaudiOnlineTests.runOutBuffer()"
#
$MINITERM TanServer@${HOST} -e "export UTGID=${NODENAME}/TANServer; exec -a \${UTGID} $gaudi_run libOnlineKernel.so tan_nameserver -a -tcp -d"&
#$MINITERM ROCollect@${HOST} -e "export UTGID=${NODENAME}/ROCollect; exec -a \${UTGID} $gaudi_run libGaudiOnline.so romon_collect -gbl=ROMonitor -size=64 -delay=500 -verbose"&
#$MINITERM ROPublish@${HOST} -e "export UTGID=${NODENAME}/ROPublish; exec -a \${UTGID} $gaudi_run libGaudiOnline.so romon_dimserver -gbl=ROMonitor -size=64 -delay=500 -verbose"&
#
sleep 4
#
#  Monitors:
#
$BIGTERM MBMMon@${HOST}     -e "export UTGID=${NODENAME}/MBMMon;    exec -a \${UTGID} $gaudi_run libOnlineKernel.so mbm_mon"&
#$BIGTERM TANMon@${HOST}     -e "export UTGID=${NODENAME}/TANMon;    exec -a \${UTGID} $gaudi_run libOnlineKernel.so tanmon -c"&
#
start_task EvtProd   "import GaudiOnlineTests;GaudiOnlineTests.runEvtProd()"
start_task EvtHolder "import GaudiOnlineTests;GaudiOnlineTests.runEvtHolder()"
start_task Moore_0   "import GaudiOnlineTests;GaudiOnlineTests.runMBMRead()"
start_task Moore_1   "import GaudiOnlineTests;GaudiOnlineTests.runMBMRead()"
#start_task Moore_2   "import GaudiOnlineTests;GaudiOnlineTests.runMBMRead()"
#start_task Moore_3   "import GaudiOnlineTests;GaudiOnlineTests.runMBMRead()"
#start_task Moore_4   "import GaudiOnlineTests;GaudiOnlineTests.runMBMRead()"
#start_task Moore_5   "import GaudiOnlineTests;GaudiOnlineTests.runMBMRead()"
#start_task Moore_6   "import GaudiOnlineTests;GaudiOnlineTests.runMBMRead()"
#start_task Moore_7   "import GaudiOnlineTests;GaudiOnlineTests.runMBMRead()"
#
sleep 3
start_task Receiver "import GaudiOnlineTests;GaudiOnlineTests.runReceiver()"
start_task DiskWR   "import GaudiOnlineTests;GaudiOnlineTests.runDiskWR('OUT',True,False,'./mdfOutput.dat')"
start_task EvtServ   "import GaudiOnlineTests;GaudiOnlineTests.runEvtServer('OUT',True)"
#
sleep 4
#
start_task Sender1 "import GaudiOnlineTests;GaudiOnlineTests.runSender('${NODENAME}/Receiver')"
start_task Sender2 "import GaudiOnlineTests;GaudiOnlineTests.runSender('${NODENAME}/Receiver')"
#
start_task EvtCons1 "import GaudiOnlineTests;GaudiOnlineTests.runNetCons('${NODENAME}/EvtServ',1,0.0005)"
start_task EvtCons2 "import GaudiOnlineTests;GaudiOnlineTests.runNetCons('${NODENAME}/EvtServ',1,0.0005)"
#
# For debugging enable this and disable any other
#$MINITERM Sender@${HOST}    -e "export UTGID=${NODENAME}/DbgTask   ; cat gaudi.gdb; gdb -x gaudi.gdb $GAUDIONLINEROOT/$CMTCONFIG/Gaudi.exe" &
# $BIGTERM MBMDump@${HOST} -e "export UTGID=${NODENAME}/MBMDump; $gaudi_run libMBMDump.so mbmdump" &
# export UTGID=${NODENAME}/prod_0; $gaudi_run libGaudiOnline.so mep_producer -n=prod_0 -p=333 -s=500 -r=2
tail -n 3 ${0}
