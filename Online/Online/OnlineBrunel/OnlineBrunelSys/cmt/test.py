#!/bin/bash
. ${GAUDIONLINEROOT}/tests/cmt/preamble.sh
export NODENAME=`python -c "print '$HOST'.split('.')[0]"`
if test -z "${PYOPTS}";
  then
    export PYOPTS=${GAUDIONLINEROOT}/python:${MYBRUNELROOT}/python
    export PYTHONPATH=${PYOPTS}:${PYTHONPATH}
fi;
#
start_task()
{
    $MINITERM ${1}@${HOST}   -e "export UTGID=${NODENAME}/${1};   exec -a \${UTGID} $Class1_task -opt=command=\"${2}\""&
}
start_Brunel()
{
    start_task Brunel_0 "import gaudirun;gaudirun.run_online(['../python/BrunelOnline.py'])"
    start_task Brunel_1 "import gaudirun;gaudirun.run_online(['../python/BrunelOnline.py'])"
    start_task Brunel_2 "import gaudirun;gaudirun.run_online(['../python/BrunelOnline.py'])"
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
start_task DiskWR   "import GaudiOnlineTests;GaudiOnlineTests.runDiskWR('Output',True,False,'./mdfOutput.dat')"
#
# $BIGTERM MBMDump@${HOST} -e "export UTGID=${NODENAME}/MBMDump; $gaudi_run libMBMDump.so mbmdump" &
# start_Brunel()
# start_task Mdf2Mbm "import GaudiOnlineTests;GaudiOnlineTests.runMDF2MBM(['Events'])"
# start_task Mdf2Mbm "import GaudiOnlineTests;GaudiOnlineTests.runMDF2MBM2(['Events'])"
tail -n 5 ${0}
