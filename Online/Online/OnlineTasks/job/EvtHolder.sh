#!/bin/bash
#export UTGID=node00101_EvtHolder_1
test -n "$1" ; export TOPLEVEL=$1
test -n "$2" ; export PARTNAME=$2

cd /home/online/Online_v4r5/Online/OnlineTasks/v1r8/job

while [ $# -ne 0 ]; do
  shift 
done

export INFOOPTIONS=/group/online/dataflow/options/${PARTNAME}/${PARTNAME}_Info.opts
export OPTIONS=/group/online/dataflow/options/${PARTNAME}/${PARTNAME}_${TOPLEVEL}_HLT.opts

. /home/online/Online_v4r5/Online/OnlineTasks/v1r8/job/setupOnline.sh

${CLASS1_TASK}  -opt=${HLTOPTS}/EvtHolder.opts


