#!/bin/bash
# =========================================================================
#
#  Default script to start a dummy task only following FSM transitions
#
#  Author   M.Frank
#  Version: 1.0
#  Date:    20/05/2013
#
# =========================================================================
#
export LOGFIFO=/tmp/logSrv.fifo;
export EVENTSELECTOR_OPTIONS;
export WORKING_DIRECTORY;
cd ${WORKING_DIRECTORY};
exec -a ${UTGID} ${Class1_task} -opt=${FARMCONFIGROOT}/options/FileRawSizeMon.opts
