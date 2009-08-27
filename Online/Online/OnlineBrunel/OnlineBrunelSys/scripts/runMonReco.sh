#!/bin/bash

export UTGID
export LOGFIFO
cd `dirname $0`/../cmt
echo "ERROR `pwd`"
. setup.slc4_amd64_gcc34_dbg.vars
BASE=`cd ..;dirname $PWD`

export PYTHONPATH=/group/online/dataflow/options/${PARTITION}:${BASE}/InstallArea/python:${PYTHONPATH}
export DATAINTERFACE=`python /group/online/dataflow/scripts/getDataInterface.py`
export TAN_PORT=YES
export TAN_NODE=${DATAINTERFACE}

TASKCLASS=-tasktype=LHCb::${2}Task
TASKTYPE=${3}
DYNAMIC_OPTS=/group/online/dataflow/options
export OPTIONSDIR=${DYNAMIC_OPTS}/${PARTITION}
export PARTITIONOPTS=${OPTIONSDIR}/${PARTITION}_Info.opts
OPTIONS=${OPTIONSDIR}/${1}
echo "ERROR $OPTIONS "

export DIM_DNS_NODE=${4}
export ONLINETASKS=/group/online/dataflow/templates
export MSGSVC=LHCb::FmcMessageSvc
export ONLINETASKS=/group/online/dataflow/templates
export DATAFLOWDIR=/group/online/dataflow
export RELEASE_DIR=${DATAFLOWDIR}/cmtuser/Online;
export PREAMBLE_OPTS=${DATAFLOWDIR}/templates/options/Preamble.opts

###export SQLITEDBPATH=/dev/shm/SQL
export gaudi_exe="$GAUDIONLINEROOT/$CMTCONFIG/Gaudi.exe libGaudiOnline.so OnlineTask -msgsvc=$MSGSVC "
echo "[INFO] +++ Starting MonRECO ${UTGID} of class ${TASKCLASS} ${TASKTYPE} with DNS:${DIM_DNS_NODE}"
echo ERROR exec -a ${UTGID} $gaudi_exe ${TASKCLASS} -opt=${OPTIONS} -main=$ONLINETASKS/options/Main.opts
exec -a ${UTGID} $gaudi_exe ${TASKCLASS} -opt=${OPTIONS} -main=$ONLINETASKS/options/Main.opts
exit;
