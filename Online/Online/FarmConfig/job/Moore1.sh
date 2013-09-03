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
. /group/hlt/MOORE/Moore_v20r3p1/InstallArea/scripts/runMooreHlt1Online_EFF.sh ${DIM_DNS_NODE} ${PARTITION_NAME} ${NBOFSLAVES}
