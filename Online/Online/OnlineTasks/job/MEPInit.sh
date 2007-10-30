#!/bin/bash
export UTGID
test -n "$1" ; export TOPLEVEL=$1
test -n "$2" ; export PARTNAME=$2

export INFOOPTIONS=/home/frankm/options/${PARTNAME}/${PARTNAME}_Info.opts
export OPTIONS=/home/frankm/options/${PARTNAME}/${PARTNAME}_${TOPLEVEL}_HLT.opts
shift
. ./setupClass0Online.sh


${CLASS0_TASK} -main=${HLTOPTS}/MEPInit.opts
