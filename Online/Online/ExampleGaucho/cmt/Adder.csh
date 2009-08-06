# !/bin/csh


if ( $1 == "" ) then 
  setenv UTGID "PART01_Adder_1"
  else 
    setenv UTGID $1
    shift
endif

#please change the DIM_DNS_NODE to your own! 
#Can be defined as an environment variable before calling the script or here
setenv DIM_DNS_NODE pclhcb80.cern.ch
if ( $DIM_DNS_NODE == "" ) then
echo "Start a DIM dns server and set environment variable DIM_DNS_NODE" 
exit
endif

echo "DIM_DNS_NODE set to " $DIM_DNS_NODE

setenv User_release_area /afs/cern.ch/user/e/evh/w0/newcmt

SetupProject Online v4r25


cd $GAUCHOROOT/cmt
source setup.csh


set OPTS = /afs/cern.ch/user/e/evh/w0/newcmt/Online_v4r25/Online/ExampleGaucho/options/Adder.opts
echo "Submmiting GauchoJob with UTGID " $UTGID  " and options file " $OPTS

$GAUDIONLINEROOT/$CMTCONFIG/Gaudi.exe \
$GAUDIONLINEROOT/$CMTCONFIG/libGaudiOnline.so GaudiOnline \
-runable=LHCb::OnlineRunable \
-options=$OPTS \
-msgsvc=MessageSvc \
-loop -auto
