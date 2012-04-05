#!/bin/bash
. ${GAUDIONLINEROOT}/tests/cmt/preamble.sh
export NODENAME=`python -c "print '$HOST'.split('.')[0]"`
export PYTHONPATH=`cd ..;dirname $PWD`/InstallArea/python:/group/online/dataflow/options/LHCb/RECONSTRUCTION:${PYTHONPATH};
ulimit -d 2097152
ulimit -m 2097152
ulimit -v 2097152
#
start_task()
{
    $MINITERM ${1}@${HOST} -sl 30000  -e "export UTGID=${NODENAME}/${1};   exec -a \${UTGID} $Class1_task -opt=command=\"${2}\""&
}
start_Brunel()
{
    start_task Brunel_0 "from Gaudi.Configuration import importOptions;importOptions('../python/TestBrunel.py');"
    # start_task Brunel_1 "from Gaudi.Configuration import importOptions;importOptions('../python/BrunelOnline.py');"
    # start_task Brunel_2 "from Gaudi.Configuration import importOptions;importOptions('../python/BrunelOnline.py');"
}
#
start_task MbmEvents "import GaudiOnlineTests;GaudiOnlineTests.runRecBuffer()"
#
$MINITERM TanServer@${HOST} -e "export UTGID=${NODENAME}/TANServer; exec -a \${UTGID} $gaudi_run libOnlineKernel.so tan_nameserver -a -tcp -d"&
#
sleep 4
#
#  Monitors:
#
$BIGTERM MBMMon@${HOST}     -e "export UTGID=${NODENAME}/MBMMon;    exec -a \${UTGID} $gaudi_run libOnlineKernel.so mbm_mon"&
#
#start_task DiskWR   "import GaudiOnlineTests;GaudiOnlineTests.runDiskWR('Output',True,False,'./mdfOutput.dat')"
#
# $BIGTERM MBMDump@${HOST} -e "export UTGID=${NODENAME}/MBMDump; $gaudi_run libMBMDump.so mbmdump" &
start_Brunel
# start_task Mdf2Mbm "import GaudiOnlineTests;GaudiOnlineTests.runMDF2MBM(['Events'])"
# start_task Mdf2Mbm "import GaudiOnlineTests;GaudiOnlineTests.runMDF2MBM2(['Events'])"

sleep 40

start_task Mdf2Mbm "import GaudiOnlineTests;GaudiOnlineTests.runMDF2MBMRepro(['Events'],'FID:10000001-6BB3-DE11-9269-001EC9AD0A4E')"

tail -n 5 ${0}
