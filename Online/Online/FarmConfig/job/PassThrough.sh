#!/bin/bash
# =========================================================================
#
#  Default script to start the passthrough process on a farm node.
#
#  Author   M.Frank
#  Version: 1.0
#  Date:    20/05/2013
#
# =========================================================================
#
export LD_PRELOAD=${CHECKPOINTINGROOT}/${CMTCONFIG}/libCheckpointing.so;
if test "${TAE_PROCESSING}" = "TAE";
then
    exec -a ${UTGID} ${Checkpoint_task} -opts=../options/${TASK_TYPE}_TAE.opts;
else
    exec -a ${UTGID} ${Checkpoint_task} -opts=../options/${TASK_TYPE}.opts;
fi;
#gdb --args  ${Checkpoint_task} -opts=../options/${TASK_TYPE}.opts -auto
