#!/bin/bash

### Script for setup and starting MEPErrorAdder for subfarm summing
### 
### To be run on the subfarm machine
###
### author David Svantesson

#cd /home/online/ONLINE/Online_v4r20/Online/OnlineTasks/job/
cd /home/dsvantes/cmtuser/Online_v4r20/Online/OnlineTasks/job/

#if test -n "$1" ; then
#   export UTGID=$1
#fi

# HOST is read by the algorithm to know which subfarm it sums
export HOST=$(hostname --short | awk '{ print toupper($1) }')
UTGID=${HOST}_MEPRxSTAT_1

. ./setupOnline.sh

#ONLINETASKSROOT=/home/dsvantes/cmtuser/Online_v4r20/Online/OnlineTasks
exec -a ${UTGID} ${gaudi_exe} -tasktype=LHCb::Class0Task -main=${GAUDIONLINEROOT}/options/Main.opts -opt=${ONLINETASKSROOT}/options/MEPErrorAdder.opts

