#
setenv UTGID $1
shift

#please change the DIM_DNS_NODE to your own!
setenv DIM_DNS_NODE pclhcb80.cern.ch
echo "DIM_DNS_NODE set to " $DIM_DNS_NODE

#setenv LHCBHOME /afs/cern.ch/lhcb
#source $LHCBHOME/scripts/CMT.csh
setenv User_release_area /afs/cern.ch/user/e/evh/w0/newcmt

#source $LHCBSCRIPTS/lhcbsetup.csh 
#source $LHCBSCRIPTS/CMT.csh
#setenv CMTCONFIG $CMTDEB
#setenv CMTCONFIG slc4_amd64_gcc34
SetupProject Online v4r25

source /afs/cern.ch/user/e/evh/w0/newcmt/Online_v4r25/Online/ExampleGaucho/cmt/setup.csh 
#setenv GAUDIONLINEROOT /afs/cern.ch/lhcb/software/releases/ONLINE/ONLINE_v4r12/Online/GaudiOnline/v2r19
echo "running " $GAUDIONLINEROOT/$CMTCONFIG

$GAUDIONLINEROOT/$CMTCONFIG/Gaudi.exe \
$GAUDIONLINEROOT/$CMTCONFIG/libGaudiOnline.so GaudiOnline \
-runable=LHCb::OnlineRunable \
-options=/afs/cern.ch/user/e/evh/w0/newcmt/Online_v4r25/Online/ExampleGaucho/options/MonObjectTextPrinter.opts \
-msgsvc=MessageSvc \
-loop -auto
