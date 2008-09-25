#!/bin/bash
export UTGID
export LOGFIFO
cd `dirname $0`/../cmt
. setup.slc4_amd64_gcc34_dbg.vars
BASE=`cd ..;dirname $PWD`

export PYTHONPATH=/group/online/dataflow/options/${PARTITION}:${BASE}/InstallArea/python:${PYTHONPATH}
export DATAINTERFACE=`python /group/online/dataflow/scripts/getDataInterface.py`
export TAN_PORT=YES
export TAN_NODE=${DATAINTERFACE}

TASKTYPE=${3}
export DIM_DNS_NODE=${4}
export ONLINETASKS=/group/online/dataflow/templates

echo "[WARN] +++ Starting BRUNEL ${UTGID} of class ${TASKCLASS} ${TASKTYPE} with DNS:${DIM_DNS_NODE}"
exec -a ${UTGID} \
    $GAUDIONLINEROOT/$CMTCONFIG/Gaudi.exe libGaudiOnline.so OnlineTask \
    -msgsvc=LHCb::FmcMessageSvc \
    -tasktype=LHCb::${2}Task \
    -main=$ONLINETASKS/options/Main.opts \
    -opt=command="\
import Gaudi,GaudiKernel.ProcessJobOptions;\
GaudiKernel.ProcessJobOptions.printing_level=999;\
Gaudi.gaudimain().configure(['../python/BrunelOnline.py']);"
#    -opt=command="import gaudirun;gaudirun.run_online(['../python/BrunelOnline.py'])"
exit
