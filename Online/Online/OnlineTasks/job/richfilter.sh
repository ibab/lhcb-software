#!/bin/bash
export UTGID
cd /group/rich/sw/cmtuser/Panoptes_v2r2/Rich/Panoptes/cmt
. ./setup.vars_slc4_amd64_gcc34_dbg
#
echo "Panoptes is taken from $PANOPTESROOT/$CMTCONFIG using $GAUDIONLINEROOT "
export DATAINTERFACE=`python -c "import socket;print socket.gethostbyname(socket.gethostname().split('.')[0]+'-d1')"`
export TAN_PORT=YES
export TAN_NODE=${DATAINTERFACE}
export DIM_DNS_NODE=${4}
DYNAMIC_OPTS=/group/online/dataflow/options/${PARTITION}
export PYTHONPATH=${DYNAMIC_OPTS}:${GAUDIONLINEROOT}/python:${PYTHONPATH}
# Off we go!
exec -a ${UTGID} \
    $GAUDIONLINEROOT/$CMTCONFIG/Gaudi.exe libGaudiOnline.so OnlineTask \
    -msgsvc=LHCb::FmcMessageSvc \
    -tasktype=LHCb::${2}Task \
    -main=/group/online/dataflow/templates/options/Main.opts \
    -opt=command="\
import os,Gaudi,GaudiKernel.ProcessJobOptions;\
from Gaudi.Configuration import importOptions;\
GaudiKernel.ProcessJobOptions.printing_level=999;\
importOptions(os.environ['PANOPTESROOT']+'/options/RichCalibMon.py');"

