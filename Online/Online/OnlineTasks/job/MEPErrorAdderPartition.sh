#!/bin/bash

### Script for setup and starting MEPErrorAdder for partition summing. 
### One instance started for each partition
###
### author David Svantesson

cd /group/online/latestOnline/Online/OnlineTasks/job

# List of partitions to start algorithm for
if test -n "$1" ; then
   # list given by input argument
   PARTLIST=$*
else
   # starting for all partitions at the same time
   PARTLIST=(TRG TRG_ECS ECAL OT OTA OTC MUON MUONA MUONC FEST LHCb TDET TPU TPU_ECS TT VELO VELOA VELOC IT RICH RICH1 PRS RICH2 HCAL)
fi

export HOST=$(hostname --short | awk '{ print toupper($1) }')

. ./setupOnline.sh

#ONLINETASKSROOT=/home/online/cmtuser/Online_v4r20/Online/OnlineTasks

#START ONE INSTANCE FOR EACH PARTITION
for PART in ${PARTLIST[@]}
do
	echo "Starting algorithm for" $PART
	#SET_PARTITION read by algorithm
	export SET_PARTITION=${PART}
	export UTGID=${PART}_MEPRxSTAT_1
	exec -a ${UTGID} ${gaudi_exe} -tasktype=LHCb::Class0Task -main=${GAUDIONLINEROOT}/options/Main.opts -opt=${ONLINETASKSROOT}/options/MEPErrorAdderPartition.opts&
done


