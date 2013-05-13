#!/bin/bash
#
echo `pwd`
echo `pwd`
echo `pwd`  $*
. ../cmt/setup.x86_64-slc5-gcc46-dbg.vars
. ./preamble.sh
export UTGID=${UTGID}
##echo $UTGID -- $* Invoking script: ${2}.sh
. ./${2}.sh
##
###exec -a ${UTGID} ${CONTROLLERROOT}/${CMTCONFIG}/external_fsm_client.exe -name=${UTGID}
##
