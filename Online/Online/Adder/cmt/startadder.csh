#
setenv UTGID $1
shift

#please change the DIM_DNS_NODE to your own!
#setenv DIM_DNS_NODE lxplus053.cern.ch
setenv DIM_DNS_NODE pclhcb11.cern.ch
echo "DIM_DNS_NODE set to " $DIM_DNS_NODE

setenv LHCBHOME /afs/cern.ch/lhcb
source $LHCBHOME/scripts/CMT.csh

cd ~/w0/newcmt
unsetenv CMTPATH 
unsetenv CMTPROJECTPATH 
setenvOnline v3r0
source /afs/cern.ch/user/e/evh/w0/newcmt/Online_v3r0/Online/OnlineTasks/cmt/setup.csh


/afs/cern.ch/user/e/evh/w0/newcmt/Online_v3r0/Online/GaudiOnline/slc4_ia32_gcc34/Gaudi.exe \
/afs/cern.ch/user/e/evh/w0/newcmt/Online_v3r0/Online/GaudiOnline/slc4_ia32_gcc34/libGaudiOnline.so GaudiOnline \
-runable=LHCb::OnlineRunable \
-options=/afs/cern.ch/user/e/evh/w0/newcmt/Online_v3r0/Online/Adder/options/Adder.opts \
-auto
