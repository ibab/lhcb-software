#!/bin/bash
echo "CMTCONFIG=$CMTCONFIG "
#
. `dirname ${0}`/../cmt/setup.${CMTCONFIG}.vars
#
ulimit -d 2097152
ulimit -m 2097152
ulimit -v 2097152
#
export UTGID=Brunel_0
export LOGFIFO=/tmp/logGaudi.fifo
export PARTITION=LHCb
export BASE=`cd ..;dirname $PWD`;
export NODENAME=`python -c "print '$HOST'.split('.')[0]"`
#
#
export LD_PRELOAD=${CHECKPOINTINGROOT}/${CMTCONFIG}/libCheckpointing.so
echo "LD_PRELOAD=${LD_PRELOAD}"
export PYTHONPATH=/group/online/dataflow/options/${PARTITION}/RECONSTRUCTION:${BASE}/InstallArea/python:${PYTHONPATH}
export PARTITIONOPTS=/group/online/dataflow/options/${PARTITION}/${PARTITION}_Info.opts;
#
export MSGSVC=MessageSvc
export MSGSVC=LHCb::FmcMessageSvc
#
export CHECKPOINT_FILE=${ONLINEBRUNELSYSROOT}/cmt/BrunelOnline.${CMTCONFIG}
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
importOptions('${1}');"
