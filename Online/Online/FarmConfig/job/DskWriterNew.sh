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
exec -a ${UTGID} ${Class2_task} -opts=../options/${TASK_TYPE}.opts
