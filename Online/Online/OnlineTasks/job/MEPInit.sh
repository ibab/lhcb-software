#!/bin/bash
export DEBUGGING=YES

. ./setupOnline.sh $*
rm -f /dev/shm/sem.bm_*
rm -f /dev/shm/sem.BM_*
rm -f /dev/shm/sem.MEP_*
rm -f /dev/shm/bm_*_MEP
rm -f /dev/shm/bm_*_SEND
rm -f /dev/shm/bm_*_EVENT
rm -f /dev/shm/bm_*_RESULT
if test -n "`grep Activity /group/online/dataflow/options/${PARTNAME}/HLT/OnlineEnvBase.py|grep EOF_CALIB`";
then
    exec -a ${UTGID} ${CLASS0_TASK} -main=${HLTOPTS}/MEPInit_EoFCalib.opts
else
    exec -a ${UTGID} ${CLASS0_TASK} -main=${HLTOPTS}/MEPInit${IS_TAE_RUN}.opts
fi;
