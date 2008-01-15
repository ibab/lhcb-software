#
setenv UTGID $1
shift

#please change the DIM_DNS_NODE to your own!
#setenv DIM_DNS_NODE lxplus053.cern.ch
setenv DIM_DNS_NODE pclhcb11.cern.ch
echo "DIM_DNS_NODE set to " $DIM_DNS_NODE

setenv LHCBHOME /afs/cern.ch/lhcb
source $LHCBHOME/scripts/CMT.csh

setenv User_release_area /afs/cern.ch/user/e/evh/w0/newcmt


SetupProject Online v4r5
source /afs/cern.ch/user/e/evh/w0/newcmt/Online_v4r5/Online/Saver/v1r3/cmt/setup.csh


Gaudi.exe libGaudiOnline.so GaudiOnline -options=/afs/cern.ch/user/e/evh/w0/newcmt/Online_v4r5/Online/Saver/v1r3/options/Saver.opts -loop -auto
