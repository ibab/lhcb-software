#  Generic script to setup the environment for an Online Gaudi/Gaucho task
#  
# Todo: remove hard-coded DIM_DNS_NODE
#       parametrise setup.sh scripts
#       make GaudiEnv version variable
echo Running as $(/usr/bin/whoami)
export HOME=/home/$(/usr/bin/whoami)
export CMTHOME=/lhcb/sw/slc4
export CMTCONFIG=slc4_ia32_gcc34
export CMTDEB=${CMTCONFIG}_dbg
export SHLVL=1
. /etc/profile.d/zzz_hepix.sh
#export CMTCONFIG=$CMTDEB
export CMTPATH=$HOME/cmtuser:$CMTPATH
#GaudiEnv v18r4
oldpwd=$PWD; cd $HOME
[ -z $LHCBSCRIPTS ] && export LHCBSCRIPTS=/afs/cern.ch/lhcb/scripts
. ${LHCBSCRIPTS}/ProjectEnv.sh Gaudi v19r1
. $HOME/cmtuser/Online/OnlineTasks/cmt/setup.sh
cd $oldpwd
. /etc/dim.conf
export TAN_PORT=YES
export TAN_NODE=$(hostname -f)
export test_exe=${ONLINEKERNELROOT}/${CMTCONFIG}/test.exe
export gaudi_exe="${GAUDIONLINEROOT}/${CMTCONFIG}/Gaudi.exe"
export gaudi_exe_args="libGaudiOnline.so OnlineTask" 
export MSGSVC=LHCb::DimMessageSvc
 
#echo Working Directory: $PWD
#${gaudi_exe} ${gaudi_exe_args} -main=../options/MBMinit.opts  -opt=../options/Daemon.opts -msgsvc=${MSGSVC}  &

