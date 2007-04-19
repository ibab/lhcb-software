#
#  Generic script to setup the environment for an Online Gaudi/Gaucho task
#  
# Todo: remove hard-coded DIM_DNS_NODE
#       parametrise setup.sh scripts
#       make GaudiEnv version variable
setenv UTGID $1
shift
setenv DIM_DNS_NODE pclhcb11.cern.ch

#evh export CMTHOME=/lhcb/sw/slc4
setenv CMTCONFIG slc4_ia32_gcc34

setenv SHLVL 1
#. /etc/profile.d/zzz_hepix.sh
#export CMTCONFIG=$CMTDEB
#evh export CMTPATH=$HOME/cmtuser:$CMTPATH
#GaudiEnv v18r4

#evh cd $HOME
#evh [ -z $LHCBSCRIPTS ] && export LHCBSCRIPTS=/afs/cern.ch/lhcb/scripts
#evh . ${LHCBSCRIPTS}/ProjectEnv.sh Gaudi v19r1
#evh . $HOME/cmtuser/Online/OnlineTasks/cmt/setup.sh
#evh cd $oldpwd
#evh . /etc/dim.conf

source $LHCBHOME/scripts/CMT.csh
setenv CMTPROJECTPATH $LHCBPROJECTPATH

source /afs/cern.ch/user/e/evh/w0/newcmt/Online_v3r1/Online/OnlineTasks/cmt/setup.csh

setenv INSTALLAREA /afs/cern.ch/lhcb/software/releases/ONLINE/ONLINE_v3r1/InstallArea


setenv TAN_PORT YES
setenv TAN_NODE `hostname -f`
setenv  test_exe $INSTALLAREA/$CMTCONFIG/bin/test.exe
setenv  gaudi_exe $INSTALLAREA/$CMTCONFIG/bin/Gaudi.exe
setenv  gaudi_exe_args "$INSTALLAREA/$CMTCONFIG/lib/libGaudiOnline.so OnlineTask" 
setenv MSGSVC LHCb::DimMessageSvc
 
#echo Working Directory: $PWD
$gaudi_exe $gaudi_exe_args -main=../options/MBMinit.opts  -opt=../options/Daemon.opts -msgsvc=${MSGSVC}  &

