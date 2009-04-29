#!/bin/bash
#this script should allow running of generic gaudi jobs in the eff farm
# evh 15/01/2008
#parent is the name of the subfarm - for storage options
#partname is the partition name
#runtype is the activity (empty: use DummyRead.opts)
printenv


export DEBUGGING=YES

test -n "$1" ; export PARENT=$( echo $1 | tr "[:lower:]" "[:upper:]" )

if test -n "$2" ; then
   export UTGID=$2
fi
if test -n "$3" ; then 
   export ONLINEVERSION=$3
fi

cd ${ONLINEVERSION}

export OPTIONS=/group/online/dataflow/options/${PARTNAME}/${PARTNAME}_Info.opts
# remove the args because they interfere with the cmt scripts
while [ $# -ne 0 ]; do
  shift 
done

. ./setupOnline.sh 


# if test -f /group/online/dataflow/options/${PARTNAME}/${PARTNAME}_${RUNTYPE}${IS_TAE_RUN}.opts;
#     then 
#     export USEROPTS=/group/online/dataflow/options/${PARTNAME}/${PARTNAME}_${RUNTYPE}${IS_TAE_RUN}.opts;
# fi

#echo exec -a ${UTGID} ${gaudi_exe3} -options=../options/GaudiExample.opts -loop &


export DIM_DNS_NODE=hlt01

exec -a ${UTGID} ${gaudi_exe3} -options=../options/InsertDB.opts -loop &
