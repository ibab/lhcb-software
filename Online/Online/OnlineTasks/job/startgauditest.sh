#!/bin/sh
#this is a copy of Moore.sh so I don't need to reconfigure the farms

export UTGID
test -n "$1" ; export TOPLEVEL=$1
test -n "$2" ; export PARTNAME=$2

export INFOOPTIONS=/home/frankm/options/${PARTNAME}/${PARTNAME}_Info.opts
export OPTIONS=/home/frankm/options/${PARTNAME}/${PARTNAME}_${TOPLEVEL}_HLT.opts

. ./pathsetup
. ./setupOnline.sh


${CLASS1_TASK} -opt=${HLTOPTS}/ReadMBM.opts
