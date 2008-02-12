#!/bin/bash
#export UTGID=node00101_EvtProd_1

while [ $# -ne 0 ]; do
  shift 
done

cd /home/online/Online_v4r5/Online/OnlineTasks/v1r8/job

. /home/online/Online_v4r5/Online/OnlineTasks/v1r8/job/setupOnline.sh

${CLASS1_TASK}  -opts=${HLTOPTS}/Dummy.opts

