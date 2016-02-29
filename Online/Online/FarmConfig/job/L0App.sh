#!/bin/bash
# =========================================================================
#
#  Default script to start the event reader task on the HLT farm
#
#  Author   F.Dordei
#  Version: 1.0
#  Date:    20/05/2013
#
# =========================================================================
#
export CMTCONFIG=x86_64-slc6-gcc49-opt;
export DAQ_INHIBIT_FINALIZE=ON;
export HN=`hostname -s | tr a-z A-Z`;
  ### export CMTCONFIG=x86_64-slc6-gcc48-dbg;
  ### source ${FARMCONFIGROOT}/job/runMooreHlt2Online_EFF.sh ${DIM_DNS_NODE} ${PARTITION_NAME} ${NBOFSLAVES};
. /group/hlt/sattelite/MooreOnlinePit_v24r4p1/InstallArea/runL0Online.sh ${DIM_DNS_NODE} ${PARTITION_NAME} ${NBOFSLAVES};
