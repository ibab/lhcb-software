#!/bin/bash
# =========================================================================
#
#  Default script to start the event reader task on the HLT farm
#
#  Author   M.Frank
#  Version: 1.0
#  Date:    20/05/2013
#
# =========================================================================
#
export CMTCONFIG=x86_64-slc6-gcc48-opt;
export HN=`hostname -s | tr a-z A-Z`;
if test -f "/group/online/dataflow/options/LHCb2/Nodes/${HN}_TriggerInfo.py";
then
  export PYTHONPATH=/group/online/dataflow/options/LHCb2/Nodes;
  unset MOOREONLINE_VERSION;
  export MOOREONLINE_VERSION=`python -c "import ${HN}_TriggerInfo as H; print H.MooreOnlineVersion"`;
  . /group/hlt/MOORE/${MOOREONLINE_VERSION}/InstallArea/runMooreHlt2Online_EFF.sh ${DIM_DNS_NODE} ${PARTITION_NAME} ${NBOFSLAVES};
  ##. /group/online/dataflow/cmtuser/OnlineDev_v5r20/Online/FarmConfig/job/MM2.sh ${DIM_DNS_NODE} ${PARTITION_NAME} ${NBOFSLAVES};
else
  echo "[ERROR] Cannot determine MOOREONLINE_VERSION. No file: /group/online/dataflow/options/LHCb2/Nodes/${HN}_TriggerInfo.py";
fi;
