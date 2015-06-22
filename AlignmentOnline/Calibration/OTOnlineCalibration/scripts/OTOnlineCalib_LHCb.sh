#!/bin/bash

# environment
. /group/online/dataflow/cmtuser/AlignmentRelease/setup.x86_64-slc6-gcc48-dbg.vars

# Extra stuff for the task
export UTGID;
export LOGFIFO=/tmp/logOTCalib.fifo;
export PARTITION="LHCb";
export PARTITION_NAME="LHCb";
HOSTUP=`echo $HOSTNAME | tr a-z A-Z`
export TNS_ADMIN=/sw/oracle/10.2.0.4/linux64/network/admin;
export DIM_DNS_NODE=mona09;
export PYTHONPATH=/group/online/dataflow/options/${PARTITION}/RECONSTRUCTION:$PYTHONPATH
export PYTHONPATH=/group/online/hlt/conditions/RunChangeHandler:$PYTHONPATH

# Start the task
exec -a ${UTGID} GaudiOnlineExe.exe libGaudiOnline.so OnlineTask -auto \
    -msgsvc=LHCb::FmcMessageSvc \
    -tasktype=LHCb::Class1Task \
    -main=/group/online/dataflow/templates/options/Main.opts \
    -opt=command="\
import os; from Gaudi.Configuration import importOptions;\
importOptions(os.environ['OTONLINECALIBRATIONROOT']+'/scripts/OTOnlineCalib.py');"
