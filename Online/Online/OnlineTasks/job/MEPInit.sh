#!/bin/bash

cd job
export DEBUGGING=YES

. ./setupOnline.sh $*

exec -a ${UTGID} ${CLASS0_TASK} -main=${HLTOPTS}/MEPInit${IS_TAE_RUN}.opts
