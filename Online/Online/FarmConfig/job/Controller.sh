#!/bin/bash
# =========================================================================
#
#  Farm worker node controller startup script
#
#  Author   M.Frank
#  Version: 1.0
#  Date:    20/05/2013
#
# =========================================================================
 . ../cmt/setup.x86_64-slc5-gcc46-dbg.vars
#
#    -print=${OUTPUT_LEVEL} \
#    -sleep=20 \
#
#
exec -a ${UTGID} ${CONTROLLERROOT}/${CMTCONFIG}/fsm_ctrl.exe \
    -print=4 -sleep=0 \
    -partition=${PARTITION_NAME} \
    -runinfo=${RUNINFO} \
    -mode=${MOORESTARTUP_MODE} \
    -taskconfig=${TASK_CONFIG} \
    -count=${NBOFSLAVES}
