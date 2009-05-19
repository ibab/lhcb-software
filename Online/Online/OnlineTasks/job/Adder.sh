#!/bin/bash
#cases: 1. 2 arguments: adder adds processes in a subfarm. options created by this script. $1: parent, $2 UTGID 
#       2. 2 arguments: calibration farm. Options to be edited by hand in options/AdderCalibrationfarm.opts
#       3. 3 arguments: adder adds adders in a partition. $1: parent, $2: UTGID. $3:. partition name 
#       4. 4 arguments: onlineversion
#       
#need to cd incase script is launched outside the taskmanager
#get online version

ulimit -d 1048576
ulimit -m 1048576
ulimit -v 1048576


test -n "$1" ; export PARENT=$( echo $1 | tr "[:upper:]" "[:lower:]" )

if test -n "$2" ; then
   export UTGID=$2
fi

if test -n "$3" ; then 
   export PARTNAME=$3
fi

if test -n "$4" ; then 
   export ONLINEVERSION=$4
fi

if test -n "$5" ; then 
   export TOP=$5
fi

export DIM_DNS_CLIENT_NODE=${DIM_DNS_NODE}.lbdaq.cern.ch

# if the parent > 6 chars its run inside a node
if test -z "${PARENT:6}" ; then
   export DIM_DNS_NODE=hlt01
   export DIM_DNS_CLIENT_NODE=${PARENT}.lbdaq.cern.ch
fi

echo UTGID ${UTGID} PARENT ${PARENT} PARTNAME ${PARTNAME} ONLINE ${ONLINEVERSION}

cd ${ONLINEVERSION}
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


echo DIM_DNS_NODE ${DIM_DNS_NODE} DIM_DNS_CLIENT_NODE ${DIM_DNS_CLIENT_NODE}

if test -n "${TOP}" ; then 
  #top level adder for this partition
  export DIM_DNS_NODE=mona08
  export DIM_DNS_CLIENT_NODE=hlt01.lbdaq.cern.ch
  exec -a ${UTGID} ${gaudi_exe3} -options=../options/Adder.opts &  
fi

if [[ ${PARENT} == "cald07" ]] ; then 
   export DIM_DNS_NODE=cald07.lbdaq.cern.ch
fi



if test -z "${TOP}" ; then
  #adder for this subfarm
  #if test -f ${SUBFARM} ; 
  #  then rm ${SUBFARM} ;
  #fi  
  if [[ ${PARENT} == "cald07" ]]
    then ${gaudi_exe3} -options=../options/AdderCalibrationfarm.opts  &
    else 
       if [[ ${PARENT} == "mona09" ]]
         then ${gaudi_exe3} -options=../options/AdderRecBrunel.opts  &
       else exec -a ${UTGID} ${gaudi_exe3} -options=../options/AdderSubfarm.opts &
#        else /usr/bin/valgrind --tool=callgrind ${gaudi_exe3} -options=../options/AdderSubfarm.opts -loop &
       fi
  fi
fi
