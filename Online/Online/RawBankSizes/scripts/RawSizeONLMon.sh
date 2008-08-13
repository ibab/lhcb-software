#!/bin/bash
export UTGID
cd /home/beat/cmtuser/Online_v4r11/Online/RawBankSizes/cmt
. ./pathsetup_32.sh

TASKCLASS=-tasktype=LHCb::${2}Task
TASKTYPE=${3}
DYNAMIC_OPTS=/group/online/dataflow/options
OPTIONS=${DYNAMIC_OPTS}/${PARTITION}/${1}
export ONLINETASKS=/group/online/dataflow/templates
export PREAMBLE_OPTS=$ONLINETASKS/options/Preamble.opts
export DATAINTERFACE=`python -c "import socket;print socket.gethostbyname(socket.gethostname().split('.')[0]+'-d1')"`
export TAN_PORT=YES
export TAN_NODE=${DATAINTERFACE}
export gaudi_exe="$GAUDIONLINEROOT/$CMTCONFIG/Gaudi.exe libGaudiOnline.so OnlineTask -msgsvc=LHCb::FmcMessageSvc "
export DIM_DNS_NODE=${4}
$gaudi_exe ${TASKCLASS} -opt=${OPTIONS} -main=$ONLINETASKS/options/Main.opts
