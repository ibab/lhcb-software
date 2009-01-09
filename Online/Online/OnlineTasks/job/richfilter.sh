#!/bin/bash
export UTGID
export PARTITION=LHCb

#cd /group/rich/sw/cmtuser/Panoptes_v1r5/Rich/Panoptes/v1r5/cmt
cd /home/ukerzel/cmtuser_PanoptesDevel/Panoptes_v1r5/Rich/Panoptes/v1r5/cmt
. ./setup.vars_slc4_amd64_gcc34

echo "Panoptes is taken from $PANOPTESROOT "

#TASKCLASS=-tasktype=LHCb::${2}Task
TASKCLASS=-tasktype=LHCb::Class1Task
#TASKTYPE=${3}
TASKTYPE=Panoptes
DYNAMIC_OPTS=/group/online/dataflow/options
#OPTIONS=/group/online/calibrationfarm/${1}
OPTIONS=/home/ukerzel/cmtuser_PanoptesDevel/Panoptes_v1r5/Rich/Panoptes/v1r5/options/RichCalibMonitor.opts
export ONLINETASKS=/group/online/dataflow/templates
export INFOOPTS=/group/online/dataflow/options/${PARTITION}/${PARTITION}_Info.opts
export PREAMBLE_OPTS=$ONLINETASKS/options/Preamble.opts
export DATAINTERFACE=`python -c "import socket;print socket.gethostbyname(socket.gethostname().split('.')[0]+'-d1')"`
export LOGFIFO=/tmp/logGaudi.fifo
export TAN_PORT=YES
export TAN_NODE=${DATAINTERFACE}
export gaudi_exe="$GAUDIONLINEROOT/$CMTCONFIG/Gaudi.exe libGaudiOnline.so OnlineTask -msgsvc=LHCb::FmcMessageSvc "
#export DIM_DNS_NODE=${4}
export DIM_DNS_NODE=cald07
#echo $gaudi_exe ${TASKCLASS} -opt=${OPTIONS} -main=$ONLINETASKS/options/Main.opts
#$gaudi_exe ${TASKCLASS} -opt=${OPTIONS} -main=$ONLINETASKS/options/Main.opts

#for PVSS
exec -a ${UTGID} $gaudi_exe ${TASKCLASS} -opt=${OPTIONS} -main=/group/online/dataflow/templates/options/Main.opts

