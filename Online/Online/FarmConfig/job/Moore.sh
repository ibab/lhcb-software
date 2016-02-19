#!/bin/bash
# =========================================================================
#
#  Default script to start MOORE on a farm node.
#
#  Author   M.Frank
#  Version: 1.0
#  Date:    20/08/2013
#
# =========================================================================
#
export CMTCONFIG=x86_64-slc6-gcc49-opt;
. /group/hlt/MOORE/${MOOREONLINE_VERSION}/InstallArea/runMooreOnline_EFF.sh ${DIM_DNS_NODE} ${PARTITION_NAME} ${NBOFSLAVES}
