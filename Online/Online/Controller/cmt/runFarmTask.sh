#!/bin/bash
#
. setup.x86_64-slc5-gcc46-dbg.vars
export UTGID=${UTGID}
exec -a ${UTGID} ../${CMTCONFIG}/external_fsm_client.exe -name=${UTGID}
