#!/bin/bash

cd job
export DEBUGGING=YES

. ./setupOnline.sh $*
rm -rf /dev/shm/sem.bm_*
rm -rf /dev/shm/sem.BM_*
rm -rf /dev/shm/sem.MEP_*
rm -rf /dev/shm/bm_*

exec -a ${UTGID} ${CLASS0_TASK} -main=${HLTOPTS}/MEPInit${IS_TAE_RUN}.opts
