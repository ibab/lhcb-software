#!/bin/bash
echo "CMTCONFIG=$CMTCONFIG "
#
ulimit -d 2097152
ulimit -m 2097152
ulimit -v 2097152
#
export LOGFIFO=/tmp/logGaudi.fifo
export LD_PRELOAD=${ONLINEBRUNELROOT}/../InstallArea/${CMTCONFIG}/lib/libCheckpointing.so
export LD_PRELOAD=${CHECKPOINTINGROOT}/${CMTCONFIG}/libCheckpointing.so
#
export MSGSVC=MessageSvc
export MSGSVC=LHCb::FmcMessageSvc
#
`which GaudiCheckpoint.exe` libGaudiOnline.so OnlineTask \
-checkpoint \
-msgsvc=$MSGSVC \
-tasktype=LHCb::Class1Task \
-main=$GAUDIONLINEROOT/options/Main.opts \
-opt=command="\
import Gaudi,GaudiKernel.ProcessJobOptions;\
from Gaudi.Configuration import importOptions;\
GaudiKernel.ProcessJobOptions.printing_level=999;\
importOptions('../python/BrunelRepro.py');"
