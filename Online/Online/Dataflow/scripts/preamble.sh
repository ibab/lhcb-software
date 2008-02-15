#!/bin/bash
if [ -r /etc/sysconfig/dim ]; then
   . /etc/sysconfig/dim
   export DIM_DNS_NODE
fi
if [ -z $DIM_DNS_NODE ]; then
   echo DIM_DNS_NODE undefined and /etc/sysconfig/dim not readable
   # exit 1
fi;
export DATAFLOWDIR=/group/online/dataflow
export RELEASE_DIR=${DATAFLOWDIR}/cmtuser/Online_v4r5
. ${RELEASE_DIR}/MyOnline/cmt/setup.vars.deb
#. /home/frankm/cmtuser/Online_v4r5/MyOnline/cmt/setup.vars.deb

export UTGID
export DATAINTERFACE=`python ${DATAFLOWDIR}/scripts/getDataInterface.py`
export TAN_PORT=YES
export TAN_NODE=${DATAINTERFACE}
export ONLINETASKS=/home/frankm/OnlineTasks
export PREAMBLE_OPTS=${DATAFLOWDIR}/templates/options/Preamble.opts

export MSGSVC=MessageSvc
export MSGSVC=LHCb::FmcMessageSvc

export test_exe="$ONLINEKERNELROOT/$CMTCONFIG/test.exe "
export gaudi_exe="$GAUDIONLINEROOT/$CMTCONFIG/Gaudi.exe libGaudiOnline.so OnlineTask -msgsvc=$MSGSVC "
export gaudi_run="$GAUDIONLINEROOT/$CMTCONFIG/Gaudi.exe libGaudiOnline.so OnlineStart "

export gaudi_exe2="$GAUDIONLINEROOT/$CMTCONFIG/Gaudi.exe libGaudiOnline.so OnlineTask -msgsvc=MessageSvc -auto "
export MINITERM='xterm  -ls -132 -geometry 132x12 -title '
export BIGTERM='xterm  -ls -132 -geometry 132x45 -title '
export WIDETERM='xterm  -ls -132 -geometry 160x50 -title '
