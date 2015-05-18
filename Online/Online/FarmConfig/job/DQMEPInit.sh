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
exec -a ${UTGID} ${Class0_task} -opt=../options/Daemon.opts -main=../options/DQ/${TASK_TYPE}.opts
