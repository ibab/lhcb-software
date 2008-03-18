#!/bin/bash
#need to cd incase script is launched outside the taskmanager
#get online version

pathofthisfile=`which $0`
first=${pathofthisfile#*_}
export onlineversion=${first%%/*}
second=${first#*/*/*/}
export onlinetasksversion=${second%%/*}

cd /home/online/Online_${onlineversion}/Online/OnlineTasks/${onlinetasksversion}/job
export DEBUGGING=YES

export $1="NOTAN"$1 


. ./setupOnline.sh $*

if test -n "$PARTNAME" 
   then export DIM_DNS_NODE=hlt01;
fi


${gaudi_exe3} -options=../options/InsertDB.opts -loop &
