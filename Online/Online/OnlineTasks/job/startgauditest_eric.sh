#!/bin/bash
#export UTGID=$1
#shift

#please change the DIM_DNS_NODE to your own!
#all gaudi jobs in farm use the same dns
export DIM_DNS_NODE=ecs01
shift
#setenv DIM_DNS_NODE pclbonsrv01.cern.ch
echo "DIM_DNS_NODE set to " $DIM_DNS_NODE

export MYSITEROOT=/sw/lib
export LHCBHOME=/sw/lib
export CMTCONFIG=slc4_ia32_gcc34

. $LHCBHOME/scripts/ExtCMT.sh
export CMTPROJECTPATH=$LHCBPROJECTPATH
cd /home/online/Online_v4r2/Online/ExampleGaucho/v2r3/cmt
. setup.sh


#export PATH=/lhcb/software/releases/lhcb/ONLINE/ONLINE_v3r1/InstallArea/slc4_ia32_gcc34/bin/:${PATH}
#export LD_LIBRARY_PATH=/lhcb/software/releases/lhcb/LHCB/LHCB_v22r2/DAQ/MDF/v2r4/slc4_ia32_gcc34/lib/:/lhcb/software/releases/lhcb/ONLINE/ONLINE_v3r1/InstallArea/slc4_ia32_gcc34/lib/:${PATH}:${LD_LIBRARY_PATH}

#echo "PATH "$PATH
#echo "LD_LIBRARY_PATH "$LD_LIBRARY_PATH
#echo "CMTPROJECTPATH "$CMTPROJECTPATH

/sw/lib/lhcb/ONLINE/ONLINE_v4r2/InstallArea/slc4_ia32_gcc34/bin/Gaudi.exe \
/sw/lib/lhcb/ONLINE/ONLINE_v4r2/InstallArea/slc4_ia32_gcc34/lib/libGaudiOnline.so GaudiOnline \
-runable=LHCb::OnlineRunable \
-options=/home/online/Online_v4r2/Online/ExampleGaucho/v2r3/options/GaudiExample.opts \
-loop

