#!/bin/bash
#  Generic script to setup the environment for an Online Gaudi/Gaucho task
#  
# Todo: remove hard-coded DIM_DNS_NODE
#       parametrise setup.sh scripts
#       make GaudiEnv version variable
#modified 20/03/2007 by EvH for tests in B157
#         18/10/2007 for Online v4r0

test -n "$1" ; export NO_TAN=$1
echo "NO_TAN="$NO_TAN
shift
oldpwd=$PWD; 
echo Running as $(/usr/bin/whoami) with DIM_DNS_NODE $DIM_DNS_NODE
[ -z $HOME ] && export HOME=/home/$(/usr/bin/whoami)
export MYSITEROOT=/sw/lib
export LHCBHOME=/sw/lib
export CMTCONFIG=slc4_ia32_gcc34

. $MYSITEROOT/scripts/ExtCMT.sh
export CMTPROJECTPATH=$LHCBPROJECTPATH
export CMTCONFIG=$CMTDEB
# export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:$LHCBHOME/lhcb/ONLINE/ONLINE_v4r5/InstallArea/slc4_ia32_gcc34/lib:$LHCBHOME/lhcb/LHCB/LHCB_v23r2/InstallArea/slc4_ia32_gcc34/lib
export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:$LHCBHOME/lhcb/ONLINE/ONLINE_v4r5/InstallArea/${CMTCONFIG}/lib:$LHCBHOME/lhcb/LHCB/LHCB_v23r2/InstallArea/${CMTCONFIG}/lib

. /home/online/Online_v4r5/Online/OnlineTasks/v1r8/cmt/setup.sh

cd $oldpwd


# message are send via the DIM Messageservice
#export MSGSVC=MessageSvc
export MSGSVC=LHCb::FmcMessageSvc

# TAN - do not touche
if test -z "$NO_TAN"
   then export TAN_PORT=YES;export DATAINTERFACE=`python -c "import socket;print socket.gethostbyname(socket.gethostname().split('.')[0]+'-d1')"`;export TAN_NODE=${DATAINTERFACE}
fi
#
# shortcuts for starting jobs

export test_exe=${ONLINEKERNELROOT}/${CMTCONFIG}/test.exe
export gaudi_run="$GAUDIONLINEROOT/$CMTCONFIG/Gaudi.exe $GAUDIONLINEROOT/$CMTCONFIG/libGaudiOnline.so OnlineStart "
export gaudi_exe="$GAUDIONLINEROOT/$CMTCONFIG/Gaudi.exe $GAUDIONLINEROOT/$CMTCONFIG/libGaudiOnline.so OnlineTask -msgsvc=$MSGSVC "  
export gaudi_exe2="$GAUDIONLINEROOT/$CMTCONFIG/Gaudi.exe $GAUDIONLINEROOT/$CMTCONFIG/libGaudiOnline.so OnlineTask -msgsvc=MessageSvc "  
export gaudi_exe3="$GAUDIONLINEROOT/$CMTCONFIG/Gaudi.exe $GAUDIONLINEROOT/$CMTCONFIG/libGaudiOnline.so GaudiOnline"  

export HLTOPTS=${ONLINETASKSROOT}/hltopts
export CLASS0_TASK="${gaudi_exe} -tasktype=LHCb::Class0Task -opt=${GAUDIONLINEROOT}/options/Daemon.opts"
export CLASS1_TASK="${gaudi_exe} -main=${HLTOPTS}/Main.opts"
export CLASS2_TASK="${gaudi_exe} -main=${HLTOPTS}/Main.opts"

#echo Working Directory: $PWD
#${gaudi_exe} ${gaudi_exe_args} -main=../options/MBMinit.opts  -opt=../options/Daemon.opts -msgsvc=${MSGSVC}  &
