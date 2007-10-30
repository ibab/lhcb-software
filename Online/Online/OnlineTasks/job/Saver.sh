#!/bin/bash
#export node=$1
test -n $1 ; export TOPLEVEL =$1
test -n $2 ; export UTGID=$2
test -n $3 ; export PART_NAME=$3

shift
cd /home/online/Online_v4r0/Online/OnlineTasks/v1r5/job
. ./setupOnline.sh
#${gaudi_exe3} -options=../options/Saver$node.opts -loop &
${gaudi_exe3} -options=../options/Saver.opts -loop &

