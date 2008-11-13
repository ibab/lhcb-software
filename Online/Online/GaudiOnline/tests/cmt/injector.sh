#!/bin/bash
. `dirname ${0}`/preamble.sh
#rm /dev/shm/bm_* /dev/shm/sem.bm_* /dev/shm/TAN* /dev/shm/sem.TAN*
export NODENAME=`python -c "print '$HOST'.split('.')[0]"`

if test -z "${PYOPTS}";
  then
    export PYOPTS=${GAUDIONLINEROOT}/python
    export PYTHONPATH=${PYOPTS}:${PYTHONPATH}
fi;
start_py_task()
{
  $MINITERM ${1}@${HOST}   -e "export UTGID=LHCb_${NODENAME}_${1};   exec -a \${UTGID} $Class1_task -opt=command=\"${2}\""&
}
start_py_task2()
{
  export UTGID=${NODENAME}_${1};   
  $Class1_task -opt=command="${2}"&
}
#
#
$BIGTERM MBMMon@${HOST}     -e "export UTGID=${NODENAME}/MBMMon;    exec -a \${UTGID} $gaudi_run libOnlineKernel.so mbm_mon"&
#
start_py_task  MBM      "import GaudiOnlineTests as g;g.runBuffer('Events')"
#
/bin/sleep 10
#
start_py_task  Transfer_00 "import GaudiOnlineTests as g;g.runDimReader('Events')"
start_py_task  Transfer_01 "import GaudiOnlineTests as g;g.runDimReader('Events')"
start_py_task  Transfer_02 "import GaudiOnlineTests as g;g.runDimReader('Events')"
start_py_task  Reader_00   "import GaudiOnlineTests as g;g.runDimReader('Events')"
start_py_task  Reader_01   "import GaudiOnlineTests as g;g.runDimReader('Events')"
start_py_task  Reader_02   "import GaudiOnlineTests as g;g.runDimReader('Events')"
start_py_task  Sorter_00   "import GaudiOnlineTests as g;g.runDimReader('Events')"
start_py_task  Sorter_01   "import GaudiOnlineTests as g;g.runDimReader('Events')"
start_py_task  Sorter_02   "import GaudiOnlineTests as g;g.runDimReader('Events')"
start_py_task  Writer_00   "import GaudiOnlineTests as g;g.runDimReader('Events')"
#
#
# For debugging enable this and disable any other
#$MINITERM Sender@${HOST}    -e "export UTGID=${NODENAME}/DbgTask   ; cat gaudi.gdb; gdb -x gaudi.gdb $GAUDIONLINEROOT/$CMTCONFIG/Gaudi.exe" &
# $BIGTERM MBMDump@${HOST} -e "export UTGID=${NODENAME}/MBMDump; $gaudi_run libMBMDump.so mbmdump" &
# export UTGID=${NODENAME}/prod_0; $gaudi_run libGaudiOnline.so mep_producer -n=prod_0 -p=333 -s=500 -r=2
tail -n 3 ${0}
