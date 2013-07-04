#!/bin/bash
# =========================================================================
#
#  Default script to start any task on the HLT farm.
#
#  Author   M.Frank
#  Version: 1.0
#  Date:    20/05/2013
#
# =========================================================================
#
if [ -r /etc/sysconfig/dim ]; then
   . /etc/sysconfig/dim
   export DIM_DNS_NODE=`echo $DIM_DNS_NODE | tr a-z A-Z`
fi
if [ -z $DIM_DNS_NODE ]; then
   echo DIM_DNS_NODE undefined and /etc/sysconfig/dim not readable
   # exit 1
fi;
#
export STATIC_OPTS=../options
export DYNAMIC_OPTS=/group/online/dataflow/options/${PARTITION_NAME}
export DATAINTERFACE=`python /group/online/dataflow/scripts/getDataInterface.py`
export ONLINETASKS=/group/online/dataflow/templates;
export INFO_OPTIONS=${DYNAMIC_OPTS}/${PARTITION_NAME}_Info.opts;
export SUBFARM_OPTIONS=${DYNAMIC_OPTS}/${PARTITION_NAME}_${DIM_DNS_NODE}_HLT.opts;
#
#
#
. ./preamble.sh;
. ./${TASK_TYPE}.sh;
