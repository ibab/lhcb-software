#!/bin/bash
export UTGID=$1;
export MOOREONLINE_VSN=$2;
export DIM_DNS_NODE=$3;
export PARTITION=$4;
export NBOFSLAVES=$5;
echo "Moore: $* Partition:${PARTITION}  DNS:${DIM_DNS_NODE}  UTGID=${UTGID}";

export LD_PRELOAD=${CHECKPOINTINGROOT}/${CMTCONFIG}/libCheckpointing.so;
export Checkpoint_task="GaudiCheckpoint.exe libGaudiOnline.so OnlineTask -msgsvc=LHCb::FmcMessageSvc -tasktype=LHCb::Class1Task -main=../options/Main.opts "
exec -a ${UTGID} ${Checkpoint_task} -opts=../options/PassThrough.opts -auto;
