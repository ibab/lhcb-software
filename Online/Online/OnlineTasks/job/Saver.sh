#!/bin/bash
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

cd /home/online/ONLINE/Online_v4r18dev/Online/OnlineTasks/job
export DEBUGGING=YES

# remove the args because they interfere with the cmt scripts
while [ $# -ne 0 ]; do
  shift 
done

export OPTIONS=/group/online/dataflow/options/${PARTNAME}/${PARTNAME}_Info.opts
export INFOOPTIONS=/group/online/dataflow/options/${PARTNAME}/${PARTNAME}_Info.opts;
echo "options "${OPTIONS}


if test -n "${TOP}" 
   then export DIM_DNS_NODE=hlt01;
fi


. ./setupOnline.sh 

if [[ ${PARENT} == "cald07" ]]
  then ${gaudi_exe3} -options=../options/SaverCalibrationfarm.opts &
  else 
     if [[ ${PARENT} == "mona08" ]]
       then ${gaudi_exe4} -options=../options/SaverMonitorfarm.opts &
       else exec -a ${UTGID} ${gaudi_exe3} -options=../options/Saver.opts &

     fi 
fi  
