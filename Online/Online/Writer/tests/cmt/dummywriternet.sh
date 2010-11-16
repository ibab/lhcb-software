#!/bin/bash

#. `dirname ${0}`/preamble.sh

DBG=0
if [ $# -ge 1 ]; then
        if [ $1 == "dbg" ]; then
                DBG=1
        fi
fi
mkdbgscr () {
cat > ./writernetgdb.x <<EOF
file ${GAUDIONLINEROOT}/${CMTCONFIG}/Gaudi.exe
b 'LHCb::MDFWriterNet::initialize()'
run libGaudiOnline.so OnlineTask -msgsvc=MessageSvc -auto -opt=./WRT.opts 
EOF
#       file ${GAUDIONLNEROOT}/${CMTCONFIG}/Gaudi.exe
#       b LHCb::MEPRxSvc::initialize
#       run libGaudiOnline.so OnlineTask -msgsvc=MessageSvc -auto -opt=$ONLINETASKSROOT/options/MEPRxSvc.opts -main=$GAUDIONLINEROOT/options/Main.opts

}


export UTGID=dummy
. ./preamble.sh

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
#
#
#  Monitors:
#
#$BIGTERM MBMMon@${HOST}  -e "export UTGID=${NODENAME}/MBMMon;    exec $gaudi_run libOnlineKernel.so mbm_mon"&

NBWRITER=1

if [ $# -ne 0 ]; then
    NBWRITER=$1
fi


echo $CMTCONFIG
echo $gaudi_exe2

export LD_LIBRARY_PATH=/sw/lib/lcg/external/gcc/4.3.2/x86_64-slc5/lib64/:$LD_LIBRARY_PATH
#UTGID=${NODENAME}/dummyWriter valgrind --tool=callgrind $Class1_task -opt=./dummyWRT.opts
UTGID=${NODENAME}/dummyWriter $Class1_task -opt=./dummyWRT.opts

#${gaudi_exe2} -opt=./WRT.opts -main=$GAUDIONLINEROOT/options/Main.opts



#start_py_task Monitor "import GaudiOnlineTests;GaudiOnlineTests.runBufferCons('Events', True, False)"

#
#tail -n 3 ${0}
# start_py_task Mdf2Mbm "import GaudiOnlineTests;GaudiOnlineTests.runMDF2MBMFile(['Events'],fname='root:/daqarea/lhcb/data/2008/RAW/TEST/test.raw')"
