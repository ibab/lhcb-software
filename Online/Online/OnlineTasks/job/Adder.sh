#!/bin/bash
#cases: 1. 2 arguments: adder adds processes in a subfarm. options created by this script. $1: parent, $2 UTGID 
#       2. 2 arguments: calibration farm. Options to be edited by hand in options/AdderCalibrationfarm.opts
#       3. 3 arguments: adder adds adders in a partition. $1: parent, $2: UTGID. $3:. partition name
#       
#need to cd incase script is launched outside the taskmanager
#get online version


test -n "$1" ; export PARENT=$( echo $1 | tr "[:upper:]" "[:lower:]" )

if test -n "$2" ; then
   export UTGID=$2
fi

if test -n "$3" ; then 
   export PARTNAME=$3
fi

if test -n "$4" ; then 
   export TOP=$4
fi
echo UTGID ${UTGID} PARENT ${PARENT} PARTNAME ${PARTNAME}

cd /home/online/Online_v4r9/Online/OnlineTasks/v1r11/job
export DEBUGGING=YES

# remove the args because they interfere with the cmt scripts
while [ $# -ne 0 ]; do
  shift 
done

export OPTIONS=/group/online/dataflow/options/${PARTNAME}/${PARTNAME}_Info.opts
echo "options "${OPTIONS}
#export SUBFARM=/group/online/dataflow/options/${PARENT}.opts
# don't have write access to this directory
#export SUBFARM=../options/Adder${PARENT}.opts

. ./setupOnline.sh

if test -n "${TOP}" ; then 
  #top level adder for this partition
  export DIM_DNS_NODE=hlt01
  ${gaudi_exe3} -options=../options/Adder.opts -loop &  
fi

if [[ ${PARENT} == "hlta08" ]] ; then 
   export DIM_DNS_NODE=hlta08.lbdaq.cern.ch
fi

echo DIM_DNS_NODE ${DIM_DNS_NODE}

if test -z "${TOP}" ; then
  #adder for this subfarm
  #if test -f ${SUBFARM} ; 
  #  then rm ${SUBFARM} ;
  #fi  
  if [[ ${PARENT} == "hlta08" ]]
    then ${gaudi_exe3} -options=../options/AdderCalibrationfarm.opts -loop &
    else 
       if [[ ${PARENT} == "mona08" ]]
         then ${gaudi_exe3} -options=../options/AdderMonitorfarm.opts -loop &
         else ${gaudi_exe3} -options=../options/AdderSubfarm.opts -loop &
       fi
  fi
fi
