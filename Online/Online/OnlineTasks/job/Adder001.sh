#!/bin/bash
export node=$1
shift

export DIM_DNS_NODE=hlte08

export MYSITEROOT=/sw/lib
export LHCBHOME=/sw/lib
export CMTCONFIG=slc4_ia32_gcc34

. $LHCBHOME/scripts/ExtCMT.sh
export CMTPROJECTPATH=$LHCBPROJECTPATH
cd /sw/lib/lhcb/ONLINE/ONLINE_v3r1/Online/Adder/v4r1/cmt
. setup.sh

/sw/lib/lhcb/ONLINE/ONLINE_v3r1/InstallArea/slc4_ia32_gcc34/bin/Gaudi.exe \
/sw/lib/lhcb/ONLINE/ONLINE_v3r1/InstallArea/slc4_ia32_gcc34/lib/libGaudiOnline.so \
GaudiOnline \
-runable=LHCb::OnlineRunable \
-options=/sw/lib/lhcb/ONLINE/ONLINE_v3r1/Online/OnlineTasks/v1r3/options/Adder$node.opts \
-loop
