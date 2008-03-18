#!/bin/bash
#cases: 1. 2 arguments: adder adds processes in a subfarm. options created by this script. $1: parent, $2 UTGID 
#       2. 3 arguments: adder adds adders in a partition. $1: parent, $2: UTGID. $3:. partition name
#need to cd incase script is launched outside the taskmanager
#get online version


test -n "$1" ; export PARENT=$1

if test -n "$3" ; then 
   export PARTNAME=$3
   test -n "$2" ; export UTGID=$2
fi

echo UTGID ${UTGID}

#pathofthisfile=`which $0`
#first=${pathofthisfile#*_}
#export onlineversion=${first%%/*}
#second=${first#*/*/*/}
#export onlinetasksversion=${second%%/*}

cd /home/online/Online_v4r6/Online/OnlineTasks/v1r9/job
export DEBUGGING=YES

# remove the args because they interfere with the cmt scripts
while [ $# -ne 0 ]; do
  shift 
done

export OPTIONS=/group/online/dataflow/options/${PARTNAME}/${PARTNAME}_Info.opts
export PARTOPTIONS=`pwd`/../options/Adder${PARENT}.opts
export SUBFARM=/group/online/dataflow/options/${PARENT}.opts

. ./setupOnline.sh

if test -n "${PARTNAME}" ; then 
  echo partname n ${PARTNAME}
  export DIM_DNS_NODE=hlt01
  ${gaudi_exe3} -options=../options/Adder.opts -loop &  
fi
if test -z "${PARTNAME}" ; then
  echo partname z ${PARTNAME}
  if test -f ${SUBFARM} ; 
    then rm ${SUBFARM} ;
  fi  
  export SUBFARMCONTENTS="Subfarm.Name = {\""$PARENT"\"}; \n Subfarm.ServerName = \""$PARENT"\"; \n Subfarm.ClientName = \""$PARENT"\";"
  echo  -e ${SUBFARMCONTENTS} > ${SUBFARM}

  ${gaudi_exe3} -options=../options/AdderSubfarm.opts -loop &
fi
