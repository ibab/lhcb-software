#!/bin/bash
. `dirname ${0}`/preamble.sh

if test -z "${PYOPTS}";
  then
    export PYOPTS=${GAUDIONLINEROOT}/python
    export PYTHONPATH=${PYOPTS}:${PYTHONPATH}
fi;

#rm /dev/shm/bm_* /dev/shm/sem.bm_* /dev/shm/TAN* /dev/shm/sem.TAN*
export NODENAME=`python -c "print '$HOST'.split('.')[0]"`
if test -z "${PYOPTS}";
  then
    export PYOPTS=${GAUDIONLINEROOT}/python
    export PYTHONPATH=${PYOPTS}:${PYTHONPATH}
fi;
#
start_py_task MbmEvents "import GaudiOnlineTests;GaudiOnlineTests.runBuffer(buffer='Events')"
#
sleep 6
#
#  Monitors:
#
$BIGTERM MBMMon@${HOST}     -e "export UTGID=${NODENAME}/MBMMon;    exec $gaudi_run libOnlineKernel.so mbm_mon"&

start_py_task Monitor "import GaudiOnlineTests;GaudiOnlineTests.runBufferCons('Events', True, False)"

#
tail -n 3 ${0}
# start_py_task Mdf2Mbm "import GaudiOnlineTests;GaudiOnlineTests.runMDF2MBMFile(['Events'],fname='root:/daqarea/lhcb/data/2008/RAW/LHCb/BEAM/32484/032484_0000081651.raw')"
