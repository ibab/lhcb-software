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
#
#    -print=${OUTPUT_LEVEL} \
#    -sleep=20 \
#
#
export PRINT_LEVEL=4;
#export PRINT_LEVEL=3;
exec -a ${UTGID} ${CONTROLLERROOT}/${CMTCONFIG}/fsm_ctrl.exe \
    -print=${PRINT_LEVEL} -sleep=0 \
    -partition=${PARTITION_NAME} \
    -runinfo=${RUNINFO} \
    -mode=${MOORESTARTUP_MODE} \
    -taskconfig=${TASK_CONFIG} \
    -count=${NBOFSLAVES}
