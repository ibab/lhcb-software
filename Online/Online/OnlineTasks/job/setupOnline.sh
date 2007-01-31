#  Generic script to setup the environment for an Online Gaudi/Gaucho task
#  
# Todo: remove hard-coded DIM_DNS_NODE
#       parametrise setup.sh scripts
#       make GaudiEnv version variable
echo Running as $(/usr/bin/whoami)
[ -z $HOME ] && export HOME=/home/$(/usr/bin/whoami)
[ -z $CMTHOME ] && export CMTHOME=/lhcb/sw/slc4
export CMTCONFIG=slc4_ia32_gcc34
export CMTDEB=${CMTCONFIG}_dbg
export PATH=/bin:/usr/bin:/usr/local/bin
#export CMTCONFIG=$CMTDEB
export CMTPATH=
export SHLVL=1
set -x 
. /etc/profile.d/zzz_hepix.sh

oldpwd=$PWD; cd $HOME
[ -z $LHCBSCRIPTS ] && export LHCBSCRIPTS=/afs/cern.ch/lhcb/scripts
[ -z $LHCBHOME ] && export LHCBHOME=/afs/cern.ch/lhcb
if  [ -z $CMTPROJECTPATH ] ; then
	echo CMTPROJECTPATH must be defined
	return 1
fi 
# . ${LHCBSCRIPTS}/CMT.sh
export CMTCONFIG=slc4_ia32_gcc34
. ${LHCBSCRIPTS}/ProjectEnv.sh Gaudi v19r1
export CMTCONFIG=slc4_ia32_gcc34
. $HOME/cmtuser/Online/OnlineTasks/cmt/setup.sh
cd $oldpwd
if [ -z $DIM_DNS_NODE ] ; then
	if [ -r /etc/dim.conf ] ; then
   		. /etc/dim.conf
	fi
else
	echo DIM_DNS_NODE not defined. Cowardly refusing to continue
	return 1
fi
# 
# GaudiOnline definitions
#

# message are send via the DIM Messageservice
export MSGSVC=MessageSvc
export MSGSVC=LHCb::DimMessageSvc

# TAN - do not touche
export TAN_PORT=YES
export TAN_NODE=$(hostname -f)

# shortcuts for starting jobs
export test_exe=${ONLINEKERNELROOT}/${CMTCONFIG}/test.exe
export gaudi_run="${GAUDIONLINEROOT}/${CMTCONFIG}/Gaudi.exe libGaudiOnline.so OnlineStart "
export gaudi_exe="${GAUDIONLINEROOT}/${CMTCONFIG}/Gaudi.exe libGaudiOnline.so OnlineTask -msgsvc=${MSGSVC} -auto"  
export gaudi_exe2="${GAUDIONLINEROOT}/${CMTCONFIG}/Gaudi.exe libGaudiOnline.so OnlineTask -msgsvc=MessageSvc -auto"  
 
#echo Working Directory: $PWD
#${gaudi_exe} ${gaudi_exe_args} -main=../options/MBMinit.opts  -opt=../options/Daemon.opts -msgsvc=${MSGSVC}  &

