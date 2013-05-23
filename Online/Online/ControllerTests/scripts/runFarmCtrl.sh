#!/bin/bash
# =========================================================================
#
#  Farm worker node controller startup script
#
#  Author   M.Frank
#  Version: 1.0
#  Date:    20/05/2013
#
# =========================================================================
. ${CONTROLLERTESTSROOT}/cmt/setup.x86_64-slc5-gcc46-dbg.vars
export UTGID=${UTGID}
exec -a ${UTGID} ${CONTROLLERROOT}/${CMTCONFIG}/fsm_ctrl.exe $*
