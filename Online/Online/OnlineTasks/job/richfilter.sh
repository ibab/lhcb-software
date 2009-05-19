#!/bin/bash

cd /home/ukerzel/cmtuser_PanoptesDevel/Panoptes_v2r3/Rich/Panoptes/cmt
#cd /group/rich/sw/cmtuser/Panoptes_v2r3/Rich/Panoptes/cmt
. ./setup.vars_slc4_amd64_gcc34_CalibFarm

#
# export environment variables
# for logger and partition
#
export UTGID
export LOGFIFO=/tmp/logGaudi.fifo
export PARTITION="LHCb"

echo "Panoptes is taken from $PANOPTESROOT/$CMTCONFIG using $GAUDIONLINEROOT "
echo "LOGFIFO is taken from $LOGFIFO"
echo "UTGID = ${UTGID}"
echo "PARTITION = ${PARTITION}"

#
# export environment variables controlling the job
#
export DATAINTERFACE=`python -c "import socket;print socket.gethostbyname(socket.gethostname().split('.')[0]+'-d1')"`
export TAN_PORT=YES
export TAN_NODE=${DATAINTERFACE}
export DIM_DNS_NODE=cald07
DYNAMIC_OPTS=/group/online/dataflow/options/${PARTITION}
export PYTHONPATH=${DYNAMIC_OPTS}:${GAUDIONLINEROOT}/python:${PYTHONPATH}

#
# start Calibration Job 
#
exec -a ${UTGID} \
    $GAUDIONLINEROOT/$CMTCONFIG/Gaudi.exe libGaudiOnline.so OnlineTask \
    -msgsvc=LHCb::FmcMessageSvc \
    -tasktype=LHCb::Class1Task \
    -main=${ONLINETASKSROOT}/hltopts/Main.opts \
    -opt=command="\
import os,Gaudi,GaudiKernel.ProcessJobOptions;\
from Gaudi.Configuration import importOptions;\
GaudiKernel.ProcessJobOptions.printing_level=999;\
importOptions(os.environ['PANOPTESROOT']+'/options/RichCalibMon.py');"

