#Clean old processes
#killall Gaudi.exe
killall Ctest.exe
killall CCPCtest.exe
killall dns.exe

#Setup environment variables
export MALLOC_CHECK_=2
export DIM_DNS_NODE=localhost
echo "DIM_DNS_NODE set to " $DIM_DNS_NODE

#export LHCBHOME /afs/cern.ch/lhcb
#source $LHCBHOME/scripts/CMT.csh

#source /afs/cern.ch/lhcb/software/releases/ONLINE/ONLINE_v2r2/OnlineSys/v2r2/cmt/setup.csh
. $LHCb_release_area/ONLINE/ONLINE_v4r8/OnlineSys/v4r8/cmt/setup.sh
$DIMROOT/$CMTCONFIG/dns.exe&
$CCPCHISTROOT/$CMTCONFIG/CCPCtest.exe&
#$CCPCHISTROOT/$CMTCONFIG/Ctest.exe&
