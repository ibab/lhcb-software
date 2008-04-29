#!/bin/bash
#need to cd incase script is launched outside the taskmanager
#get online version

test -n "$1" ; export PARENT=$1

test -n "$2" ; export UTGID=$2

if test -n "$3" ; then 
   export PARTNAME=$3

fi

cd /home/online/Online_v4r7/Online/OnlineTasks/v1r10/job
export DEBUGGING=YES

# remove the args because they interfere with the cmt scripts
while [ $# -ne 0 ]; do
  shift 
done

. ./setupOnline.sh 

if test -n "$PARTNAME" 
   then export DIM_DNS_NODE=hlt01;
fi

${gaudi_exe3} -options=../options/Saver.opts -loop &

