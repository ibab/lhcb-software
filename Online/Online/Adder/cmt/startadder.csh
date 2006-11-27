#
setenv UTGID $1
shift

#please change the DIM_DNS_NODE to your own!
#setenv DIM_DNS_NODE lxplus053.cern.ch
setenv DIM_DNS_NODE pclhcb11.cern.ch
echo "DIM_DNS_NODE set to " $DIM_DNS_NODE

setenv LHCBHOME /afs/cern.ch/lhcb
source $LHCBHOME/scripts/CMT.csh

cd ~/w0/cmtuser
setenv CMTPATH `pwd`


OnlineEnv v2r2


source ~/w0/cmtuser/Online/Adder/v1r0/cmt/setup.csh 

source ~/w0/cmtuser/Online/GaudiOnline/v2r2/cmt/setup.csh 

$GAUDIONLINEROOT/$CMTCONFIG/Gaudi.exe \
$GAUDIONLINEROOT/$CMTCONFIG/libGaudiOnline.so \
GaudiOnline \
-runable=LHCb::OnlineRunable \
-options=/afs/cern.ch/user/e/evh/w0/cmtuser/Online/Adder/v1r0/options/Adder.opts \
-msgsvc=MessageSvc \
-loop
