#!/bin/bash
#need to cd incase script is launched outside the taskmanager
#get online version
#ulimit -c unlimited
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
echo UTGID ${UTGID} PARENT ${PARENT} PARTNAME ${PARTNAME}

cd ${ONLINEVERSION}
export DEBUGGING=YES

# remove the args because they interfere with the cmt scripts
while [ $# -ne 0 ]; do
  shift 
done

export OPTIONS=/group/online/dataflow/options/${PARTNAME}/${PARTNAME}_Info.opts
export INFOOPTIONS=/group/online/dataflow/options/${PARTNAME}/${PARTNAME}_Info.opts;



if test -n "${TOP}" 
   then export DIM_DNS_NODE=mona08;
fi

echo "options "${OPTIONS} "dim dns node "${DIM_DNS_NODE}  
if [[ ${PARTNAME} == "LHCb" ]]
   then 
      . ./setupOnline.sh 
      exec -a ${UTGID} ${CLASS1_TASK} -options=../options/pooper.opts &
   elif [[ ${PARTNAME} == "LHCb1" ]]
   then
      . ./setupOnline.sh 
      exec -a ${UTGID} ${CLASS1_TASK} -options=../options/pooper.opts &
   elif [[ ${PARTNAME} == "FEST" ]]
   then
      . ./setupOnline.sh 
      exec -a ${UTGID} ${CLASS1_TASK} -options=../options/pooper.opts &
   else 
      source /group/online/dataflow/cmtuser/Online/MyOnline/cmt/setup.x86_64-slc5-gcc46-dbg.vars
      export LOGFIFO=/tmp/logGaudi.fifo
      export gaudi_exe="${GAUDIONLINEROOT}/${CMTCONFIG}/Gaudi.exe ${GAUDIONLINEROOT}/${CMTCONFIG}/libGaudiOnline.so OnlineTask -msgsvc=LHCb::FmcMessageSvc"
      exec -a ${UTGID}  ${gaudi_exe} -tasktype=LHCb::Class2Task -options=/group/online/dataflow/templates/options/FSMDummyTask.opts -main=/group/online/dataflow/templates/options/Main.opts &
fi
