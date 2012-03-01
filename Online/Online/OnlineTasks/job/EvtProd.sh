#!/bin/bash
#cd job
export DEBUGGING=YES

. ./setupOnline.sh $*

exec -a ${UTGID} ${CLASS1_TASK}  -opts=${HLTOPTS}/EvtProd${IS_TAE_RUN}.opts
