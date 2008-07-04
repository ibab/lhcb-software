#!/bin/bash


export UTGID=FM

cd /home/online/Online_v4r10/Online/OnlineTasks/v1r12/job
export DEBUGGING=YES

# remove the args because they interfere with the cmt scripts
while [ $# -ne 0 ]; do
  shift 
done

/home/online/Online_v4r10/Online/FM/v1r0/cmt/setup.sh

. ./setupOnline.sh


${gaudi_exe3} -options=../options/FM.opts -loop -auto &
