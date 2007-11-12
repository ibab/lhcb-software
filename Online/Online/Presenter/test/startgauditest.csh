#
killall Gaudi.exe
setenv UTGID $1
shift

#please change the DIM_DNS_NODE to your own!
#setenv DIM_DNS_NODE pclhcb11.cern.ch
#setenv DIM_DNS_NODE pclbonsrv01.cern.ch
echo "DIM_DNS_NODE set to " $DIM_DNS_NODE


#setenv LHCBHOME /afs/cern.ch/lhcb
#source $LHCBHOME/scripts/CMT.csh

#source $LHCBSCRIPTS/lhcbsetup.csh 
#source $LHCBSCRIPTS/CMT.csh

#setenv CMTPATH `pwd`
#setenv CMTCONFIG $CMTDEB

#echo 'gaudisvcshr  ' $GaudiSvcShr

#$ONLINEKERNELROOT/$CMTCONFIG/gentest.exe $GaudiOnlineShr.so OnlineDeamon -dll=libOnlineKernel.so -call=rtl_test_main
#$GAUDIONLINEROOT/$CMTCONFIG/Gaudi.exe $GaudiOnlineShr.so GaudiOnline -runable=LHCb::OnlineRunable -options=~/cmtuser/Online/GauchoJob/v2r3/options/ExampleGaucho.opts -msgsvc=MessageSvc

 source $ONLINETASKSROOT/cmt/setup.csh
$GAUDIONLINEROOT/$CMTCONFIG/Gaudi.exe \
$GAUDIONLINEROOT/$CMTCONFIG/libGaudiOnline.so GaudiOnline \
-runable=LHCb::OnlineRunable \
-options=$PRESENTERROOT/test/GaudiExample.opts \
-msgsvc=MessageSvc \
-loop \
-auto 
