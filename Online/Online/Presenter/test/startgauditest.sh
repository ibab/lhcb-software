#!/bin/bash
export DIM_DNS_NODE=localhost
#. $LHCb_release_area/ONLINE/ONLINE_v4r20/OnlineSys/v4r20/cmt/setup.sh
#. $User_release_area/Online/Gaucho/v6r0/cmt/setup.sh
#. $User_release_area/Online/ExampleGaucho/v3r0/cmt/setup/sh
killall Gaudi.exe
export UTGID=$1
shift

#please change the DIM_DNS_NODE to your own!
echo "DIM_DNS_NODE set to " $DIM_DNS_NODE

. $ONLINETASKSROOT/cmt/setup.sh
$GAUDIONLINEROOT/$CMTCONFIG/Gaudi.exe \
$GAUDIONLINEROOT/$CMTCONFIG/libGaudiOnline.so GaudiOnline \
-runable=LHCb::OnlineRunable \
-options=$PRESENTERROOT/test/GaudiExample.opts \
-msgsvc=MessageSvc \
-loop \
-auto 
