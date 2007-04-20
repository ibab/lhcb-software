setenv UTGID eee
#Clean old processes
killall Gaudi.exe
killall DNS.exe

#Setup environment variables
setenv MALLOC_CHECK_ 2
setenv DIM_DNS_NODE $HOST
source $LHCBHOME/scripts/CMT.csh
setenvOnline v3r1
setenv CMTCONFIG $CMTDEB

#Fire up DIM DNS
source $LHCb_release_area/ONLINE/ONLINE_v3r1/Online/DIM/v16r7/cmt/setup.csh
$LHCb_release_area/ONLINE/ONLINE_v3r1/Online/DIM/v16r7/$CMTCONFIG/DNS.exe&

#Get service components running
#source $GAUDIONLINEROOT/cmt/setup.csh
#source $GAUCHOROOT/cmt/setup.csh
#source $EXAMPLEGAUCHOROOT/cmt/setup.csh
#source $EXAMPLEGAUCHOROOT/job/startgauditest.csh eee&
source  ~/cmtuser/Online_v3r1/Online/GaudiOnline/v2r3/cmt/setup.csh
#source $LHCb_release_area/ONLINE/ONLINE_v3r1/Online/GaudiOnline/v2r6/cmt/setup.csh
source  ~/cmtuser/Online_v3r1/Online/Gaucho/v5r5/cmt/setup.csh
#source $LHCb_release_area/ONLINE/ONLINE_v3r1/Online/Gaucho/v5r4/cmt/setup.csh
source  ~/cmtuser/Online_v3r1/Online/ExampleGaucho/v2r2/cmt/setup.csh
#source $LHCb_release_area/ONLINE/ONLINE_v3r1/Online/ExampleGaucho/v2r2/cmt/setup.csh


#source ~/cmtuser/Online_v3r1/Online/ExampleGaucho/v2r2/job/startgauditest.csh eee&

source /afs/cern.ch/lhcb/software/releases/ONLINE/ONLINE_v3r1/OnlineSys/v3r1/cmt/setup.csh
source $GAUCHOJOBROOT/cmt/setup.csh

echo GAUCHOJOBROOT = $GAUCHOJOBROOT
echo EXAMPLEGAUCHOROOT = $EXAMPLEGAUCHOROOT

set OPTS = $GAUCHOJOBROOT/options/GauchoJob.opts
echo "Submmiting GauchoJob with UTGID " $UTGID  " and options file " $OPTS

setenv MSGSVGTYPE MessageSvc
$GAUDIONLINEROOT/$CMTCONFIG/Gaudi.exe \
$GAUDIONLINEROOT/$CMTCONFIG/libGaudiOnline.so GaudiOnline \
-runable=LHCb::OnlineRunable \
-options=../GaudiExample.opts \
-msgsvc=MessageSvc \
-loop -auto&

