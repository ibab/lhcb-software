#!/bin/bash
#export node=$1


test -n $1 ; export TOPLEVEL=$1
test -n $2 ; export UTGID=$2
test -n $3 ; export PART_NAME=$3

# remove the args because they interfere with the cmt scripts
while [ $# -ne 0 ]; do
  shift 
done

if test -n "$PARTNAME" 
   then export DIM_DNS_NODE=hlt01;
fi

cd /home/online/Online_v4r5/Online/OnlineTasks/v1r8/job
. /home/online/Online_v4r5/Online/OnlineTasks/v1r8/job/setupOnline.sh NOTAN

#${gaudi_exe3} -options=../options/InsertDB$node.opts -loop &
${gaudi_exe3} -options=../options/InsertDB.opts -loop &
