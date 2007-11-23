#!/bin/bash
#export node=$1
#need to cd incase script is launched outside the taskmanager
cd /home/online/Online_v4r2/Online/OnlineTasks/v1r6/job
test -n "$1" ; export TOPLEVEL=$1
test -n "$2" ; export UTGID=$2
test -n "$3" ; export PARTNAME=$3
export OPTIONS=/group/online/dataflow/options/${PARTNAME}/${PARTNAME}_Info.opts
export PARTOPTIONS=`pwd`/../options/Adder${TOPLEVEL}.opts
echo "PARTOPTIONS "$PARTOPTIONS
shift

. ./setupAdderOnline.sh
if test -n "$PARTNAME" 
  then ${gaudi_exe3} -options=../options/Adder.opts -loop &
fi
if test -z "$PARTNAME" 
  then ${gaudi_exe3} -options=../options/Adder$TOPLEVEL.opts -loop &
fi
