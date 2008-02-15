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
export RELEASE_DIR=/group/online/dataflow/cmtuser/Gaudi_v19r2
export DATAINTERFACE=`python ${DATAFLOWDIR}/scripts/getDataInterface.py`
export TAN_PORT=YES
export TAN_NODE=${DATAINTERFACE}
