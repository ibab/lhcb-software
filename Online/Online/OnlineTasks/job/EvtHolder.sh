#!/bin/bash
#export UTGID=node00101_EvtHolder_1
test -n "$1" ; export TOPLEVEL=$1
test -n "$2" ; export PARTNAME=$2

export INFOOPTIONS=/group/online/dataflow/options/${PARTNAME}/${PARTNAME}_Info.opts
export OPTIONS=/group/online/dataflow/options/${PARTNAME}/${PARTNAME}_${TOPLEVEL}_HLT.opts
shift
. ./setupOnline.sh

${CLASS1_TASK}  -opt=${HLTOPTS}/EvtHolder.opts


