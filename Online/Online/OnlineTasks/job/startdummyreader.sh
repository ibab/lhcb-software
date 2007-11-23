#!/bin/sh
#this is a copy of Moore.sh so I don't need to reconfigure the farms

test -n "$1" ; export TOPLEVEL=$1
test -n "$2" ; export PARTNAME=$2

export INFOOPTIONS=/group/online/dataflow/options/${PARTNAME}/${PARTNAME}_Info.opts
export OPTIONS=/group/online/dataflow/options/${PARTNAME}/${PARTNAME}_${TOPLEVEL}_HLT.opts

. /home/online/Online_v4r2/Online/OnlineTasks/v1r6/job/setupOnline.sh


${CLASS1_TASK} -opt=${HLTOPTS}/DummyRead.opts
