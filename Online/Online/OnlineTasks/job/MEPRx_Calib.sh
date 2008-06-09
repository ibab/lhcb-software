#!/bin/bash
#export UTGID=node00101_MEPRx_1

export DEBUGGING=true
cd job
. ./setupOnline.sh $*

export TELL1BOARDS=/group/online/dataflow/options/${PARTNAME}/${PARTNAME}_Tell1Boards.opts

${CLASS1_TASK} -opt=${HLTOPTS}/MEPRxSvc_Calib.opts
