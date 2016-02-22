#!/bin/bash
cd /group/online/dataflow/cmtuser
export User_release_area=`pwd`;
export CMTPROJECTPATH=`pwd`:${CMTPROJECTPATH};
source OnlineRelease/setup.x86_64-slc6-gcc49-dbg.vars
cd OnlineRelease/Online/FarmConfig/job
export msg_svc=LHCb::FmcMessageSvc
export HOST=`hostname`
export UTGID=GEN_${HOST}_BusyPub
export LOGFIFO=/tmp/logGaudi.fifo
GaudiOnlineExe.exe libGaudiOnline.so OnlineTask -msgsvc=$msg_svc -tasktype=LHCb::Class2Task -main=/group/online/dataflow/templates/options/Main.opts -opts=../options/BusyPub.opts -auto
