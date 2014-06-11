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
. /group/hlt/MOORE/${MOOREONLINE_VERSION}/InstallArea/runMooreHlt2Online_EFF.sh ${DIM_DNS_NODE} ${PARTITION_NAME} ${NBOFSLAVES};
