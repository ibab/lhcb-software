#Clean old processes
#killall Gaudi.exe
killall Ctest.exe
killall DNS.exe

#Setup environment variables
export MALLOC_CHECK_=2
export DIM_DNS_NODE=localhost
echo "DIM_DNS_NODE set to " $DIM_DNS_NODE

#export LHCBHOME /afs/cern.ch/lhcb
#source $LHCBHOME/scripts/CMT.csh

#exportOnline v3r2

#source /afs/cern.ch/lhcb/software/releases/ONLINE/ONLINE_v2r2/OnlineSys/v2r2/cmt/setup.csh
. $LHCb_release_area/ONLINE/ONLINE_v4r0/OnlineSys/v4r0/cmt/setup.sh
$DIMROOT/$CMTCONFIG/DNS.exe&
$CCPCHISTROOT/$CMTCONFIG/CCPCtest.exe&
