#!/bin/bash
#  Generic script to setup the environment for an Online Gaudi/Gaucho task
#  
# Todo: remove hard-coded DIM_DNS_NODE
#       parametrise setup.sh scripts
#       make GaudiEnv version variable
#modified 20/03/2007 by EvH for tests in B157
#         18/10/2007 for Online v4r0

echo Running as $(/usr/bin/whoami) with DIM_DNS_NODE $DIM_DNS_NODE
#[ -z $HOME ] && export HOME=/home/$(/usr/bin/whoami)

. pathsetup

#export MSGSVC=MessageSvc

# TAN - do not touche
export TAN_PORT=YES
# export TAN_NODE=$(hostname -f)
export DATAINTERFACE=`python -c "import socket;print socket.gethostbyname(socket.gethostname().split('.')[0]+'-d1')"`
export TAN_NODE=${DATAINTERFACE}
#
# shortcuts for starting jobs
#next ones redundant? fixes export limit evh 30/10/2007
#export test_exe=${ONLINEKERNELROOT}/${CMTCONFIG}/test.exe
#export gaudi_run="$GAUDIONLINEROOT/$CMTCONFIG/Gaudi.exe $GAUDIONLINEROOT/$CMTCONFIG/libGaudiOnline.so OnlineStart "
#export gaudi_exe="$GAUDIONLINEROOT/$CMTCONFIG/Gaudi.exe $GAUDIONLINEROOT/$CMTCONFIG/libGaudiOnline.so OnlineTask -msgsvc=$MSGSVC "  
export gaudi_exe="$GAUDIONLINEROOT/$CMTCONFIG/Gaudi.exe $GAUDIONLINEROOT/$CMTCONFIG/libGaudiOnline.so OnlineTask -msgsvc=MessageSvc "  
#export gaudi_exe2="$GAUDIONLINEROOT/$CMTCONFIG/Gaudi.exe $GAUDIONLINEROOT/$CMTCONFIG/libGaudiOnline.so OnlineTask -msgsvc=MessageSvc "  
#export gaudi_exe3="$GAUDIONLINEROOT/$CMTCONFIG/Gaudi.exe $GAUDIONLINEROOT/$CMTCONFIG/libGaudiOnline.so GaudiOnline"  

export HLTOPTS=${ONLINETASKSROOT}/hltopts
#export CLASS0_TASK="${gaudi_exe} -opt=${GAUDIONLINEROOT}/options/Daemon.opts"
export CLASS1_TASK="${gaudi_exe} -main=${HLTOPTS}/Main.opts"
#identical?
#export CLASS2_TASK="${gaudi_exe} -main=${HLTOPTS}/Main.opts"

#echo Working Directory: $PWD
#${gaudi_exe} ${gaudi_exe_args} -main=../options/MBMinit.opts  -opt=../options/Daemon.opts -msgsvc=${MSGSVC}  &

