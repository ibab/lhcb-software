#!/bin/bash
export UTGID=$1
export PARTNAME=$2;
export DIM_DNS_NODE=$3;

export INFOOPTIONS=/group/online/dataflow/options/${PARTNAME}/${PARTNAME}_Info.opts;


#. SetupProject.sh Online v4r33
#doesn't work from script do by hand


#setenv GAUDIONLINEROOT /afs/cern.ch/lhcb/software/releases/ONLINE/ONLINE_v4r12/Online/GaudiOnline/v2r19
echo running $GAUDIONLINEROOT/$CMTCONFIG with utgid $UTGID partition $PARTNAME dimdns $DIM_DNS_NODE

$GAUDIONLINEROOT/$CMTCONFIG/Gaudi.exe \
$GAUDIONLINEROOT/$CMTCONFIG/libGaudiOnline.so GaudiOnline \
-runable=LHCb::OnlineRunable \
-options=../options/InsertDB.opts \
-msgsvc=MessageSvc \
-loop -auto
