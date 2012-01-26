#!/bin/bash
#cd job
export DEBUGGING=YES

. ./setupOnline.sh $*

exec -a ${UTGID} ${CLASS1_TASK}  -opts=${HLTOPTS}/OvrWr${IS_TAE_RUN}.opts

