#!/bin/bash
. ${GAUDIONLINEROOT}/tests/cmt/preamble.sh
export NODENAME=`python -c "print '$HOST'.split('.')[0]"`
export PYTHONPATH=`cd ..;dirname $PWD`/InstallArea/python:${PYTHONPATH};
export Standalone_test=1
#
start_task()
{
    $MINITERM ${1}@${HOST} -sl 30000  -e "export UTGID=${NODENAME}/${1};   exec -a \${UTGID} $Class1_task -opt=command=\"${2}\""&
}
start_Brunel()
{
    start_task Brunel_0 "from Gaudi.Configuration import importOptions;importOptions('../python/BrunelOnline.py');"
    start_task Brunel_1 "from Gaudi.Configuration import importOptions;importOptions('../python/BrunelOnline.py');"
    start_task Brunel_2 "from Gaudi.Configuration import importOptions;importOptions('../python/BrunelOnline.py');"
}
#
start_task MbmReco "import GaudiOnlineTests;GaudiOnlineTests.runRecBuffer2()"
#
$MINITERM TanServer@${HOST} -e "export UTGID=${NODENAME}/TANServer; exec -a \${UTGID} $gaudi_run libOnlineKernel.so tan_nameserver -a -tcp -d"&
#
sleep 4
#
#  Monitors:
#
$BIGTERM MBMMon@${HOST}     -e "export UTGID=${NODENAME}/MBMMon;    exec -a \${UTGID} $gaudi_run libOnlineKernel.so mbm_mon"&
#
start_task FarmRecv     "import GaudiOnlineTests;GaudiOnlineTests.runReceiver(buffer='Input')"
start_task StorageRecv  "import GaudiOnlineTests;GaudiOnlineTests.runReceiver(buffer='Recv')"
sleep 3
start_task Storage2Farm "import GaudiOnlineTests;GaudiOnlineTests.runReproSender('${HOST}/FarmRecv',buffer='Send')"
start_task Farm2Storage "import GaudiOnlineTests;GaudiOnlineTests.runReproSender('${HOST}/StorageRecv',buffer='Output')"
#start_task RecoWR   "import GaudiOnlineTests;GaudiOnlineTests.runRecoWR('Recv',True,False,'./mdfOutput.dat')"
#
# $BIGTERM MBMDump@${HOST} -e "export UTGID=${NODENAME}/MBMDump; $gaudi_run libMBMDump.so mbmdump" &
start_Brunel
# start_task Mdf2Mbm "import GaudiOnlineTests;GaudiOnlineTests.runMDF2MBM(['Send'])"
# start_task Mdf2Mbm "import GaudiOnlineTests;GaudiOnlineTests.runMDF2MBM2(['Send'])"
# start_task Mdf2Mbm "import GaudiOnlineTests;GaudiOnlineTests.runMDF2MBMRepro(['Send'],'FID:A010D1EC-6BB3-DE11-9269-001EC9AD0A4E')"
# start_task Mdf2Mbm "import data,GaudiOnlineTests;GaudiOnlineTests.runMDF2MBMRepro(['Send'],data.Input)"
tail -n 5 ${0}
