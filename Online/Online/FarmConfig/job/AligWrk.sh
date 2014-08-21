#!/bin/bash
# =========================================================================
#
#  Default script to start the data writer task on a farm node
#
#  Author   M.Frank
#  Version: 1.0
#  Date:    20/05/2013
#
# =========================================================================
#
#export LD_LIBRARY_PATH=/home/beat/cmtuser/myOnline/InstallArea/${CMTCONFIG}/lib:$LD_LIBRARY_PATH

exec -a ${UTGID} ${Class1_task} -opts=../options/${TASK_TYPE}.opts
