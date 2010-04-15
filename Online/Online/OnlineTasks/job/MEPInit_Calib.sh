#!/bin/bash

#cd job
export DEBUGGING=YES

. ./setupOnline.sh $*
rm -f /dev/shm/sem.bm_*
rm -f /dev/shm/sem.BM_*
rm -f /dev/shm/sem.MEP_*
rm -f /dev/shm/bm_*_MEP
rm -f /dev/shm/bm_*_SEND
rm -f /dev/shm/bm_*_EVENT
rm -f /dev/shm/bm_*_RESULT

exec -a ${UTGID} ${CLASS0_TASK} -main=${HLTOPTS}/MEPInit${IS_TAE_RUN}_Calib.opts
