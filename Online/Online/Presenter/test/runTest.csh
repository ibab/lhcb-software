#Clean old processes
killall DNS.exe
killall Ctest.exe

#Setup environment variables
setenv MALLOC_CHECK_ 2
setenv DIM_DNS_NODE $HOST
source $LHCBHOME/scripts/CMT.csh
setenvOnline v3r2
#setenv CMTCONFIG $CMTDEB
source Online/Presenter/v0r1/cmt/setup.csh
source $LHCb_release_area/ONLINE/ONLINE_v3r2/OnlineSys/v3r2/cmt/setup.csh

#Fire up DIM DNS
source $DIMROOT/cmt/setup.csh
$DIMROOT/$CMTCONFIG/DNS.exe&
sleep 2s
#Get service components running
$CCPCHISTROOT/$CMTCONFIG/Ctest.exe&

source $ONLINETASKSROOT/cmt/setup.csh
${GAUDIONLINEROOT}/${CMTCONFIG}/Gaudi.exe ${GAUDIONLINEROOT}/${CMTCONFIG}/libGaudiOnline.so GaudiOnline  \
  -msgsvc=MessageSvc \
    -options=$PRESENTERROOT/test/GaudiExample.opts \
      -loop

