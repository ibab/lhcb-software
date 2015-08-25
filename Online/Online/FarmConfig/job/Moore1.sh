#!/bin/bash
# =========================================================================
#
#  Default script to start the HLT1 task on the HLT farm
#
#  Author   M.Frank
#  Version: 1.0
#  Date:    20/05/2013
#
# =========================================================================
#
#. ./PassThrough.sh $*
###echo "Moore version: ${MOORE_VERSION}";
export CMTCONFIG=x86_64-slc6-gcc48-opt;
. /group/hlt/MOORE/${MOOREONLINE_VERSION}/InstallArea/runMooreHlt1Online_EFF.sh ${DIM_DNS_NODE} ${PARTITION_NAME} ${NBOFSLAVES};
#. ${FARMCONFIGROOT}/job/runMooreHlt1Online_EFF.sh ${DIM_DNS_NODE} ${PARTITION_NAME} ${NBOFSLAVES};

