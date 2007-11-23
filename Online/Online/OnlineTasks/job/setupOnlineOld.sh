#!/bin/bash
#  Generic script to setup the environment for an Online Gaudi/Gaucho task
#  
# Todo: remove hard-coded DIM_DNS_NODE
#       parametrise setup.sh scripts
#       make GaudiEnv version variable
#modified 20/03/2007 by EvH for tests in B157
#         18/10/2007 for Online v4r0

shift
echo Running as $(/usr/bin/whoami) with DIM_DNS_NODE $DIM_DNS_NODE
[ -z $HOME ] && export HOME=/home/$(/usr/bin/whoami)
#evh [ -z $CMTHOME ] && export CMTHOME=/lhcb/sw/slc4


export MYSITEROOT=/sw/lib
export LHCBHOME=/sw/lib
export CMTCONFIG=slc4_ia32_gcc34
#export CMTDEB=${CMTCONFIG}_dbg
export PATH=/bin:/usr/bin:/usr/local/bin
#export CMTCONFIG=$CMTDEB

#export CMTPATH=

#evh export SHLVL=1
#evh set -x 
#evh . /etc/profile.d/zzz_hepix.sh

oldpwd=$PWD; 

#evh cd $HOME
#evh [ -z $LHCBSCRIPTS ] && export LHCBSCRIPTS=/afs/cern.ch/lhcb/scripts
#evh [ -z $LHCBHOME ] && export LHCBHOME=/afs/cern.ch/lhcb
#evh if  [ -z $CMTPROJECTPATH ] ; then
#evh 	echo CMTPROJECTPATH must be defined
#evh	return 1
#evh fi 
# . ${LHCBSCRIPTS}/CMT.sh

#evh export CMTCONFIG=slc4_ia32_gcc34
#evh . ${LHCBSCRIPTS}/ProjectEnv.sh Gaudi v19r1
#evh export CMTCONFIG=slc4_ia32_gcc34

. $LHCBHOME/scripts/ExtCMT.sh
export CMTPROJECTPATH=$LHCBPROJECTPATH
#. ${LHCBHOME}/scripts/setenvProject.sh Online v3r1
#setenvOnline v3r1
export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:$LHCBHOME/lhcb/ONLINE/ONLINE_v4r0/InstallArea/slc4_ia32_gcc34/lib:$LHCBHOME/lhcb/LHCB/LHCB_v22r9/InstallArea/slc4_ia32_gcc34/lib
#evh . $HOME/cmtuser/Online/OnlineTasks/cmt/setup.sh
cd /home/online/Online_v4r0/Online/OnlineTasks/v1r5/cmt
. setup.sh

cd $oldpwd

#evh  if [ -z $DIM_DNS_NODE ] ; then
#evh	if [ -r /etc/dim.conf ] ; then
#evh   		. /etc/dim.conf
#evh	fi
#evh  else
#evh	echo DIM_DNS_NODE not defined. Cowardly refusing to continue
#evh	return 1
#evh  fi
# 
# GaudiOnline definitions
#

# message are send via the DIM Messageservice
# export MSGSVC=LHCb::DimMessageSvc
export MSGSVC=MessageSvc

# TAN - do not touche
export TAN_PORT=YES
# export TAN_NODE=$(hostname -f)
export DATAINTERFACE=`python -c "import socket;print socket.gethostbyname(socket.gethostname().split('.')[0]+'-d1')"`
export TAN_NODE=${DATAINTERFACE}
#
# shortcuts for starting jobs
export test_exe=${ONLINEKERNELROOT}/${CMTCONFIG}/test.exe
export gaudi_run="$GAUDIONLINEROOT/$CMTCONFIG/Gaudi.exe $GAUDIONLINEROOT/$CMTCONFIG/libGaudiOnline.so OnlineStart "
export gaudi_exe="$GAUDIONLINEROOT/$CMTCONFIG/Gaudi.exe $GAUDIONLINEROOT/$CMTCONFIG/libGaudiOnline.so OnlineTask -msgsvc=$MSGSVC "  
export gaudi_exe2="$GAUDIONLINEROOT/$CMTCONFIG/Gaudi.exe $GAUDIONLINEROOT/$CMTCONFIG/libGaudiOnline.so OnlineTask -msgsvc=MessageSvc "  
export gaudi_exe3="$GAUDIONLINEROOT/$CMTCONFIG/Gaudi.exe $GAUDIONLINEROOT/$CMTCONFIG/libGaudiOnline.so GaudiOnline"  

export HLTOPTS=${ONLINETASKSROOT}/hltopts
export CLASS0_TASK="${gaudi_exe} -opt=${GAUDIONLINEROOT}/options/Daemon.opts"
export CLASS1_TASK="${gaudi_exe} -main=${HLTOPTS}/Main.opts"
export CLASS2_TASK="${gaudi_exe} -main=${HLTOPTS}/Main.opts"

#echo Working Directory: $PWD
#${gaudi_exe} ${gaudi_exe_args} -main=../options/MBMinit.opts  -opt=../options/Daemon.opts -msgsvc=${MSGSVC}  &

