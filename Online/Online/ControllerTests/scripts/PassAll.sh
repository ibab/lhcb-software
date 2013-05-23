#!/bin/bash
#
export LD_PRELOAD=${CHECKPOINTINGROOT}/${CMTCONFIG}/libCheckpointing.so;
exec -a ${UTGID} ${Checkpoint_task} -opts=../options/PassThrough.opts
#gdb --args  ${Checkpoint_task} -opts=../options/PassThrough.opts -auto
