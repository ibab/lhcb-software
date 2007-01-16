#
setenv UTGID $1
set argv

#please change the DIM_DNS_NODE to your own!

#setenv DIM_DNS_NODE pclhcb11.cern.ch
setenv DIM_DNS_NODE lxplus005.cern.ch

echo "DIM_DNS_NODE set to " $DIM_DNS_NODE


setenv LHCBHOME /afs/cern.ch/lhcb
source $LHCBHOME/scripts/CMT.csh

OnlineEnv v2r2

source /afs/cern.ch/lhcb/software/releases/ONLINE/ONLINE_v2r2/OnlineSys/v2r2/cmt/setup.csh
source $GAUCHOJOBROOT/cmt/setup.csh

echo GAUCHOJOBROOT = $GAUCHOJOBROOT
echo EXAMPLEGAUCHOROOT = $EXAMPLEGAUCHOROOT

set OPTS = $GAUCHOJOBROOT/options/GauchoJob.opts
echo "Submmiting GauchoJob with UTGID " $UTGID  " and options file " $OPTS

setenv MSGSVGTYPE MessageSvc

$GAUCHOJOBROOT/slc3_ia32_gcc323/GauchoJob.exe $OPTS 
