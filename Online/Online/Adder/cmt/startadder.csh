#
setenv UTGID $1
shift

#please change the DIM_DNS_NODE to your own!
setenv DIM_DNS_NODE pclhcb11.cern.ch
#setenv DIM_DNS_NODE pclbonsrv01.cern.ch
echo "DIM_DNS_NODE set to " $DIM_DNS_NODE

setenv LHCBHOME /afs/cern.ch/lhcb
source $LHCBHOME/scripts/CMT.csh
#setenv CMTOPT slc4_ia32_gcc34_dbg
#setenv CMTCONFIG slc4_ia32_gcc34_dbg
setenv CMTCONFIG slc4_ia32_gcc34

#source $LHCBSCRIPTS/lhcbsetup.csh 
#source $LHCBSCRIPTS/CMT.csh
setenv User_release_area /afs/cern.ch/user/e/evh/w0/newcmt


setenvOnline v3r4
source /afs/cern.ch/user/e/evh/w0/newcmt/Online_v3r4/Online/Adder/v4r3/cmt/setup.csh

	
/afs/cern.ch/user/e/evh/w0/newcmt/Online_v3r4/InstallArea/slc4_ia32_gcc34_dbg/bin/Gaudi.exe \
/afs/cern.ch/user/e/evh/w0/newcmt/Online_v3r4/InstallArea/slc4_ia32_gcc34_dbg/lib/libGaudiOnline.so GaudiOnline \
-runable=LHCb::OnlineRunable \
-options=/afs/cern.ch/user/e/evh/w0/newcmt/Online_v3r4/Online/Adder/v4r3/options/Adder.opts
