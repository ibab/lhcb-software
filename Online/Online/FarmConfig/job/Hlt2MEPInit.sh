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
exec -a ${UTGID} ${Class0_task} -opt=../options/Daemon.opts -main=../options/${TASK_TYPE}.opts
