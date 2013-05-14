#!/bin/bash
#
. ${CONTROLLERTESTROOT}/cmt/setup.x86_64-slc5-gcc46-dbg.vars
##echo "UTGID=${UTGID}"
export UTGID=${UTGID}
exec -a ${UTGID} ${CONTROLLERROOT}/${CMTCONFIG}/fsm_ctrl.exe $*
