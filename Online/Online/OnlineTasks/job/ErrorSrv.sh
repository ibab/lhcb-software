#!/bin/bash
test -n "$1" ; export TOPLEVEL=$1
test -n "$2" ; export PARTNAME=$2

while [ $# -ne 0 ]; do
  shift 
done
cd /home/online/Online_v4r5/Online/OnlineTasks/v1r8/job

export INFOOPTIONS=/group/online/dataflow/options/${PARTNAME}/${PARTNAME}_Info.opts
export OPTIONS=/group/online/dataflow/options/${PARTNAME}/${PARTNAME}_${TOPLEVEL}_HLT.opts

. /home/online/Online_v4r5/Online/OnlineTasks/v1r8/job/setupOnline.sh
#${gaudi_exe}  -main=${GAUDIONLINEROOT}/options/ErrorSrv.opts -opt=${GAUDIONLINEROOT}/options/Daemon.opts &

${CLASS1_TASK}  -main=${ONLINETASKSROOT}/options/ErrorSrv.opts  &
