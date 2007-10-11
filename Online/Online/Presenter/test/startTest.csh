#Clean old processes
#killall Gaudi.exe
killall Ctest.exe
killall DNS.exe

#Setup environment variables
setenv MALLOC_CHECK_ 2
setenv DIM_DNS_NODE $HOST
echo "DIM_DNS_NODE set to " $DIM_DNS_NODE

setenv LHCBHOME /afs/cern.ch/lhcb
source $LHCBHOME/scripts/CMT.csh

#setenvOnline v3r2

#source /afs/cern.ch/lhcb/software/releases/ONLINE/ONLINE_v2r2/OnlineSys/v2r2/cmt/setup.csh
source $LHCb_release_area/ONLINE/ONLINE_v3r5/OnlineSys/v3r5/cmt/setup.csh
$DIMROOT/$CMTCONFIG/DNS.exe&
$CCPCHISTROOT/$CMTCONFIG/Ctest.exe&
