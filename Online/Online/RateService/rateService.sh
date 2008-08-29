#!/bin/bash
export UTGID=$1
shift

echo "UTGID set to "$UTGID

export DIM_DNS_NODE=$HOST
echo "DIM_DNS_NODE set to "$DIM_DNS_NODE

RATE_SERVICE_HOME=~/cmtuser/Online_v4r12/Online/RateService/v2r2
OPTIONS=$RATE_SERVICE_HOME/options/RateService.opts

source $RATE_SERVICE_HOME/cmt/setup.sh 


$GAUDIONLINEROOT/$CMTCONFIG/Gaudi.exe \
$GAUDIONLINEROOT/$CMTCONFIG/libGaudiOnline.so GaudiOnline \
-runable=LHCb::OnlineRunable \
-options=$OPTIONS \
-msgsvc=MessageSvc \
-loop -auto
