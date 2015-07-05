#!/bin/bash
# =========================================================================
#
#  Default script to start the buffer manager on the HLT farm worker node
#
#  Author   M.Frank
#  Version: 1.0
#  Date:    20/05/2013
#
# =========================================================================
#
HH=`hostname -s | tr a-z A-Z`;
exec -a ${UTGID} ${Class0_task} \
    -opt=${FARMCONFIGROOT}/options/Daemon.opts \
    -main=${DATAQUALITYROOT}/options/${TASK_TYPE}.opts
