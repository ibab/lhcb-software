#!/bin/bash
export PARTITION=LHCb

. /group/calo/sw/scripts/setOrwell.sh

TASKCLASS=-tasktype=LHCb::Class1Task
TASKTYPE=Orwell
if test -n "$3" ; 
  then export RUNTYPE=$3
fi  

if  [[ ${RUNTYPE:0:4} == "TAE_" ]]
  then 
    echo "TAE run";
    export IS_TAE_RUN="_TAE";
    export RUNTYPE=${RUNTYPE:4};
  else 
    echo "Normal run";export IS_TAE_RUN="";
  fi;
OPTIONS=${ORWELLOPTS}/CaloDAQCalib${IS_TAE_RUN}.opts;
echo "Options: $* "
echo "Options: ${OPTIONS}"
export ONLINETASKS=/group/online/dataflow/templates
export INFOOPTS=/group/online/dataflow/options/${PARTITION}/${PARTITION}_Info.opts
export PREAMBLE_OPTS=$ONLINETASKS/options/Preamble.opts
export DATAINTERFACE=`python -c "import socket;print socket.gethostbyname(socket.gethostname().split('.')[0]+'-d1')"`
export TAN_PORT=YES
export TAN_NODE=${DATAINTERFACE}
export gaudi_exe="$GAUDIONLINEROOT/$CMTCONFIG/Gaudi.exe libGaudiOnline.so OnlineTask -msgsvc=LHCb::FmcMessageSvc "
export DIM_DNS_NODE=cald07
exec -a ${UTGID} $gaudi_exe ${TASKCLASS} -opt=${OPTIONS} -main=$ONLINETASKSROOT/hltopts/Main.opts

