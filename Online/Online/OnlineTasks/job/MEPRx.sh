#!/bin/bash
#export UTGID=node00101_MEPRx_1

test -n "$1" ; export TOPLEVEL=$1
test -n "$2" ; export PARTNAME=$2

# remove the args because they interfere with the cmt scripts
while [ $# -ne 0 ]; do
  shift 
done

cd /home/online/Online_v4r5/Online/OnlineTasks/v1r8/job

export INFOOPTIONS=/group/online/dataflow/options/${PARTNAME}/${PARTNAME}_Info.opts
export OPTIONS=/group/online/dataflow/options/${PARTNAME}/${PARTNAME}_${TOPLEVEL}_HLT.opts
export TELL1BOARDS=/group/online/dataflow/options/${PARTNAME}/${PARTNAME}_Tell1Boards.opts

. /home/online/Online_v4r5/Online/OnlineTasks/v1r8/job/setupOnline.sh

echo "INFOOPTIONS "$INFOOPTIONS" OPTIONS "$OPTIONS" TELL1BOARDS "$TELL1BOARDS
${CLASS1_TASK} -opt=${HLTOPTS}/MEPRxSvc.opts
