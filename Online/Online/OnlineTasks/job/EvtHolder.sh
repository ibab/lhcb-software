#!/bin/bash
cd job
export DEBUGGING=YES
. ./setupOnline.sh $*
${CLASS1_TASK}  -opt=${HLTOPTS}/EvtHolder${IS_TAE_RUN}.opts



