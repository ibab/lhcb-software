#!/bin/csh
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


#!/bin/csh
setenv UTGID $1
shift

#please change the DIM_DNS_NODE to your own!
#setenv DIM_DNS_NODE pclhcb53.cern.ch
#setenv DIM_DNS_NODE pclbonsrv01.cern.ch
echo ">>> DIM_DNS_NODE set to " $DIM_DNS_NODE

echo ">>> debug 1"

#/afs/cern.ch/lhcb/scripts/CMT.zsh
/afs/cern.ch/lhcb/scripts/CMT.csh

echo ">>> HOME "$HOME


echo ">>> debug 2"
echo ">>> LD_LIBRARY_PATH "$LD_LIBRARY_PATH
echo ">>> PATH "$PATH

echo ">>> debug3"

/afs/cern.ch/lhcb/scripts/SetupProject.sh Online v4r10
#SetupProject Online v4r10

echo ">>> debug4"

cd ~/cmtuser/Online_v4r10/Online/RateService/v2r1/cmt/
source ./setup.csh 
#echo 'gaudisvcshr  ' $GaudiSvcShr

echo ">>> debug5"

echo ">>> CMTCONFIG "$CMTCONFIG

#$ONLINEKERNELROOT/$CMTCONFIG/gentest.exe $GaudiOnlineShr.so OnlineDeamon -dll=libOnlineKernel.so -call=rtl_test_main
#$GAUDIONLINEROOT/$CMTCONFIG/Gaudi.exe $GaudiOnlineShr.so GaudiOnline -runable=LHCb::OnlineRunable -options=~/cmtuser/Online/GauchoJob/v2r3/options/ExampleGaucho.opts -msgsvc=MessageSvc

# to get acces to a libGaudiKernel.so for Gaudi.exe
setenv LD_LIBRARY_PATH $LD_LIBRARY_PATH":/afs/cern.ch/user/j/jmenou/cmtuser/Online_v4r7/InstallArea/slc4_amd64_gcc34/lib"
#$LD_LIBRARY_PATH = $LD_LIBRARY_PATH":/afs/cern.ch/user/j/jmenou/cmtuser/Online_v4r7/InstallArea/slc4_amd64_gcc34/lib"
echo ">>> LD_LIBRARY_PATH "$LD_LIBRARY_PATH
#export $LS_LIBRARY_PATH

#/afs/cern.ch/lhcb/software/releases/ONLINE/ONLINE_v4r7/InstallArea/slc4_amd64_gcc34/bin/Gaudi.exe \

/afs/cern.ch/lhcb/software/releases/ONLINE/ONLINE_v4r10/Online/GaudiOnline/v2r17/slc4_amd64_gcc34/Gaudi.exe \
/afs/cern.ch/lhcb/software/releases/ONLINE/ONLINE_v4r10/Online/GaudiOnline/v2r17/slc4_amd64_gcc34/libGaudiOnline.so GaudiOnline \
-runable=LHCb::OnlineRunable \
-options=../options/RateService.opts \
-msgsvc=MessageSvc \
-loop -auto
