#!/bin/bash
#
. setup.x86_64-slc5-gcc46-dbg.vars
echo "UTGID=${UTGID}"
export UTGID=${UTGID}
##export DIM_DNS_NODE=ecs04
echo exec -a ${UTGID} ../${CMTCONFIG}/fsm_ctrl.exe $*
exec -a ${UTGID} ../${CMTCONFIG}/fsm_ctrl.exe $*
