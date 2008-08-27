# !/bin/csh
#
#**************************************************************************************
# Instructions to run GaudiExample( for ONLINE_v4r5 )
#1-Before starting the job, start the Dim dns server (in any host, eg.,lxplus011 ):
#
#        SetupProject Online v4r5
#        setenv LD_LIBRARY_PATH $DIMROOT/$CMTCONFIG/:${LD_LIBRARY_PATH}
#        $DIMROOT/$CMTCONFIG/DNS.exe >! dns.log &
#
#
#2-Start also the Did (Dim debugger, a client) that can send commands to the Gaudi dim
#server(your job) (any host):
#  a) Set the environment variables for the dns:
#
#        setenv LD_LIBRARY_PATH $DIMROOT/$CMTCONFIG/:${LD_LIBRARY_PATH}
#        setenv DIM_DNS_NODE lxplus011.cern.ch
#
#  b) Start did:
#        $DIMROOT/$CMTCONFIG/did.exe &
#
#  On did window, click view -> all servers.
#  The Gaudi online job server (the utgid is the servername) will appear when the job is running.
#  Click on it and then on services. The list of dim services and commands served by the server appears.
#
#3-Start the Gaudi online job using the script startgaudijob.csh:
#  a) Set DIM_DNS_NODE environment variable (change the host below in accordance to your choice for Dim dns):
#         setenv DIM_DNS_NODE lxplus011.cern.ch
#  b) Run the script from any directory:
#         csh  $LHCBRELEASES/ONLINE/ONLINE_v4r5/Online/ExampleGaucho/v2r4/cmt/startgaudijob.csh utgid
#
# The utgid argument should follow the convention nodename_taskname_i, e.g., lxplus215_Test_1
#
#
#4-The job will start and run. Use Did to inspect the services. 
#
#5- When you finish, stop the job through Did:
#   Click on the CMD:utgid (CMD:lxplus215_Test_1 in the example) and click on View/Send button
#   write "stop" in the window that opens and give OK.
#Attention: There is a similar command CMD:utgid/ (with a slash at the end) which has another functionality. Don't use it.)
#
#Finished!
#**************************************************************************************




if ( $1 == "" ) then 
  setenv UTGID "test"
  else {
    setenv UTGID $1
    shift
  }
endif


#please change the DIM_DNS_NODE to your own! 
#Can be defined as an environment variable before calling the script or here
setenv DIM_DNS_NODE pclhcb80.cern.ch
if ( $DIM_DNS_NODE == "" ) then
echo "Start a DIM dns server and set environment variable DIM_DNS_NODE" 
exit
endif

echo "DIM_DNS_NODE set to " $DIM_DNS_NODE

setenv LHCBHOME /afs/cern.ch/lhcb
source $LHCBHOME/scripts/CMT.csh
setenv User_release_area /afs/cern.ch/user/e/evh/w0/newcmt

SetupProject Online v4r12


cd /afs/cern.ch/user/e/evh/w0/newcmt/Online_v4r12/Online/ExampleGaucho/v3r0/cmt
source setup.csh

echo EXAMPLEGAUCHOROOT = $EXAMPLEGAUCHOROOT

set OPTS = $EXAMPLEGAUCHOROOT/options/GaudiExample.opts
echo "Submmiting GauchoJob with UTGID " $UTGID  " and options file " $OPTS

$GAUDIONLINEROOT/$CMTCONFIG/Gaudi.exe \
$GAUDIONLINEROOT/$CMTCONFIG/libGaudiOnline.so GaudiOnline \
-runable=LHCb::OnlineRunable \
-options=$OPTS \
-msgsvc=MessageSvc \
-loop -auto
