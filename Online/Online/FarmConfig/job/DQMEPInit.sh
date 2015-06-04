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
. ./DQConfig.sh;
HH=`hostname -s | tr a-z A-Z`;
if test "${HH}" = "MONA1001";then
    export MBM_CONFIG_OPTS=${FARMCONFIGROOT}/options/DQ/DQReaderMBM.opts;
else
    export MBM_CONFIG_OPTS=${FARMCONFIGROOT}/options/DQ/DQWorkerMBM.opts;
fi;
echo "MBM_CONFIG_OPTS: ${MBM_CONFIG_OPTS}";
exec -a ${UTGID} ${Class0_task} -opt=../options/Daemon.opts -main=../options/DQ/${TASK_TYPE}.opts
