#!/bin/bash
export node=$1
shift

export DIM_DNS_NODE=gw03.daq.lhcb

export MYSITEROOT=/lhcb/software/releases
export LHCBHOME=/lhcb/software/releases
export CMTCONFIG=slc4_ia32_gcc34

. $LHCBHOME/scripts/ExtCMT.sh
export CMTPROJECTPATH=$LHCBPROJECTPATH
cd /lhcb/software/releases/lhcb/ONLINE/ONLINE_v3r1/Online/Adder/v4r1/cmt
. setup.sh

/lhcb/software/releases/lhcb/ONLINE/ONLINE_v3r1/InstallArea/slc4_ia32_gcc34/bin/Gaudi.exe \
/lhcb/software/releases/lhcb/ONLINE/ONLINE_v3r1/InstallArea/slc4_ia32_gcc34/lib/libGaudiOnline.so \
GaudiOnline \
-runable=LHCb::OnlineRunable \
-options=/lhcb/software/releases/lhcb/ONLINE/ONLINE_v3r1/Online/OnlineTasks/v1r3/options/Adder$node.opts \
-loop
