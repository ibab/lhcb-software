#!/bin/bash
#export node=$1
#need to cd incase script is launched outside the taskmanager
cd /home/online/Online_v4r5/Online/OnlineTasks/v1r8/job
test -n "$1" ; export TOPLEVEL=$1
test -n "$2" ; export UTGID=$2
test -n "$3" ; export PARTNAME=$3

# remove the args because they interfere with the cmt scripts
while [ $# -ne 0 ]; do
  shift 
done

export OPTIONS=/group/online/dataflow/options/${PARTNAME}/${PARTNAME}_Info.opts
export PARTOPTIONS=`pwd`/../options/Adder${TOPLEVEL}.opts
export SUBFARM=/group/online/dataflow/options/${TOPLEVEL}.opts
#echo "PARTOPTIONS "$PARTOPTIONS


if test -n "$PARTNAME" 
   then export DIM_DNS_NODE=hlt01;
fi

. /home/online/Online_v4r5/Online/OnlineTasks/v1r8/job/setupOnline.sh NOTAN
if test -n "$PARTNAME" 
  then ${gaudi_exe3} -options=../options/Adder.opts -loop &
fi
if test -z "$PARTNAME" ; then
  #then ${gaudi_exe3} -options=../options/Adder$TOPLEVEL.opts -loop & 
  if test -f /group/online/dataflow/options/${SUBFARM} ; 
    then rm /group/online/dataflow/options/${SUBFARM} ;
  fi  
  export SUBFARMCONTENTS="Subfarm.Name = {\""$TOPLEVEL"\"};"
  echo  ${SUBFARMCONTENTS} > ${SUBFARM}
  ${gaudi_exe3} -options=../options/AdderSubfarm.opts -loop &
fi
