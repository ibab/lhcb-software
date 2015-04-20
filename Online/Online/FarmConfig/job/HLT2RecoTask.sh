#!/bin/bash
#
cd HLT2Reco;
. ./HLT2RecoTask.sh;

. ./HLT2RecoConfig.sh;
export SEND_TARGET=None;
. ./HLT2Reco/HLT2Reco.sh;
#
exec -a ${UTGID} ${Class2_task} -opts=../options/${TASK_TYPE}.opts
