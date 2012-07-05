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


. ./preamble.sh
 export LD_LIBRARY_PATH=/sw/lib/lcg/external/gcc/4.3.2/x86_64-slc5/lib64/:$LD_LIBRARY_PATH

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
echo $PYTHONPATH 
start_py_task MbmEvents "import GaudiOnlineTests;GaudiOnlineTests.runBuffer(buffer='TestWriter')"
#
sleep 6
$monitorreaderd_run
#
#  Monitors:
#
#$BIGTERM MBMMon@${HOST}  -e "export UTGID=${NODENAME}/MBMMon;    exec $gaudi_run libOnlineKernel.so mbm_mon"&

NBWRITER=1

if [ $# -ne 0 ]; then
    NBWRITER=$1
fi

echo $CMTCONFIG


if [ $DBG = 1 ]; then
        mkdbgscr
        gdb -x ./writernetgdb.x
else

    while [ $NBWRITER -gt 0 ] ; do
        $MINITERM Writer@${HOST} -e "export UTGID=${NODENAME}/Writer${NBWRITER}; exec -a \${UTGID} $Class1_task -opt=./WRT.opts"&
#        $MINITERM Writer@${HOST} -e "export UTGID=${NODENAME}/Writer${NBWRITER}; exec -a \${UTGID} /usr/bin/valgrind --tool=callgrind --log-file=valgrind.log $Class1_task -opt=./WRT.opts"&
        NBWRITER=$(($NBWRITER - 1))
    done  
fi

#${gaudi_exe2} -opt=./WRT.opts -main=$GAUDIONLINEROOT/options/Main.opts



#start_py_task Monitor "import GaudiOnlineTests;GaudiOnlineTests.runBufferCons('Events', True, False)"

#
#tail -n 3 ${0}
# start_py_task Mdf2Mbm "import GaudiOnlineTests;GaudiOnlineTests.runMDF2MBMFile(['Events'],fname='root:/daqarea/lhcb/data/2008/RAW/TEST/test.raw')"
