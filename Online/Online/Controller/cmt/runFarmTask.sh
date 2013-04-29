#!/bin/bash
#
#cd /home/frankm/cmtuser/Online_v4r68/Online/Controller/cmt
. setup.x86_64-slc5-gcc46-dbg.vars
#echo "UTGID=${UTGID}"
export UTGID=${UTGID}
exec -a ${UTGID} ../${CMTCONFIG}/external_fsm_client.exe -name=${UTGID}
