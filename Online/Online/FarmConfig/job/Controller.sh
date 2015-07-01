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
# Output:   -print=${OUTPUT_LEVEL} \
# Debug:    -sleep=20 \
#
#
export PRINT_LEVEL=4;
#export PRINT_LEVEL=2;
if test "${MOORESTARTUP_MODE}" = "RESTORE";
then
    mkdir -p /tmp/Commands; # Needed by the tasks
    HH=`hostname -s | tr a-z A-Z`;
    if test "${HH}" = "`echo ${HH} | grep -i -e hlt[a-f][0-1][0-9][0-3][0-9]`"; then
	if test "${PARTITION_NAME}" = "LHCb"; then
	    ##echo `python ${FARMCONFIGROOT}/job/ConfigureCheckpoint.py -r ${RUNINFO} -c -l` | tr ";" ";\n";
	    eval `python ${FARMCONFIGROOT}/job/ConfigureCheckpoint.py -r ${RUNINFO} -c -l`;
	elif test "${PARTITION_NAME}" = "LHCb2"; then
	    eval `python ${FARMCONFIGROOT}/job/ConfigureCheckpoint.py -r ${RUNINFO} -c -l`;
	elif test "${PARTITION_NAME}" = "FEST"; then
	    eval `python ${FARMCONFIGROOT}/job/ConfigureCheckpoint.py -r ${RUNINFO} -c -l`;
	fi;
    fi;
fi;
exec -a ${UTGID} fsm_ctrl.exe \
    -print=${PRINT_LEVEL} -sleep=0 \
    -partition=${PARTITION_NAME} \
    -runinfo=${RUNINFO} \
    -mode=${MOORESTARTUP_MODE} \
    -taskconfig=${ARCH_FILE} \
    -bindcpu=1 \
    -count=${NBOFSLAVES}
