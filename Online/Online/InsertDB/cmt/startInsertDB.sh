#!/bin/bash
export UTGID=$1
export PARTNAME=$2;
export DIM_DNS_NODE=$3;

export INFOOPTIONS=/group/online/dataflow/options/${PARTNAME}/${PARTNAME}_Info.opts;

#need to do a SetupProject Online vxxx before running this script
echo running $GAUDIONLINEROOT/$CMTCONFIG with utgid $UTGID partition $PARTNAME dimdns $DIM_DNS_NODE

$GAUDIONLINEROOT/$CMTCONFIG/Gaudi.exe \
$GAUDIONLINEROOT/$CMTCONFIG/libGaudiOnline.so GaudiOnline \
-runable=LHCb::OnlineRunable \
-options=../options/InsertDB.opts \
-msgsvc=MessageSvc \
-loop -auto
