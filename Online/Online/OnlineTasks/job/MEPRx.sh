#!/bin/bash
#export UTGID=node00101_MEPRx_1

test -n "$1" ; export TOPLEVEL=$1
test -n "$2" ; export PARTNAME=$2

export INFOOPTIONS=/home/frankm/options/${PARTNAME}/${PARTNAME}_Info.opts
export OPTIONS=/home/frankm/options/${PARTNAME}/${PARTNAME}_${TOPLEVEL}_HLT.opts
export TELL1BOARDS=/home/frankm/options/${PARTNAME}/${PARTNAME}_Tell1Boards.opts
shift
. ./setupOnline.sh
echo "INFOOPTIONS "$INFOOPTIONS" OPTIONS "$OPTIONS" TELL1BOARDS "$TELL1BOARDS
${CLASS1_TASK} -opt=${HLTOPTS}/MEPRxSvc.opts
