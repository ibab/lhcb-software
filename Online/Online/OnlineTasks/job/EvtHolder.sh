#!/bin/bash
cd job
export DEBUGGING=YES
. ./setupOnline.sh $*
exec -a ${UTGID} ${CLASS1_TASK}  -opt=${HLTOPTS}/EvtHolder${IS_TAE_RUN}.opts



