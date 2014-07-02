#!/bin/bash
export UTGID=$1;
export MOOREONLINE_VSN=$2;
export DIM_DNS_NODE=$3;
export PARTITION=$4;
export NBOFSLAVES=$5;
echo "++++ Brunel: $* Partition:${PARTITION}  DNS:${DIM_DNS_NODE}  UTGID=${UTGID} Logger:${LOGFIFO} NBOFSLAVES=${NBOFSLAVES}";
#
/group/online/dataflow/cmtuser/OnlineBrunel_v5r8/OnlineBrunelSys/scripts/runBrunelStandalone.sh \
    blabla \
    Class1 \
    Brunel \
    ${DIM_DNS_NODE} \
    -auto
