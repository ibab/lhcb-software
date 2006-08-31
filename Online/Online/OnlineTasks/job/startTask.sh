#  Generic script to start an Online Gaudi/Gaucho task
#  
echo Running as $(/usr/bin/whoami)
export HOME=/home/$(/usr/bin/whoami)
export MYSITEROOT=/lhcb/sw
export CMTHOME=/lhcb/sw
export CMTCONFIG=slc3_ia32_gcc323
export CMTDEB=slc3_ia32_gcc323_dbg
#export CMTOPT=slc3_ia32_gcc323
. $CMTHOME/scripts/ExtCMT.sh      
export CMTCONFIG=$CMTDEB
export CMTPATH=$HOME/cmtuser:$CMTPATH
#GaudiEnv v18r4
. ${LHCBSCRIPTS}/ProjectEnv.sh Gaudi v18r4
. $HOME/cmtuser/Online/OnlineKernel/cmt/setup.sh
. $HOME/cmtuser/Online/GaudiOnline/cmt/setup.sh
export DIM_DNS_NODE=ctrl03.daq.lhcb
export TAN_PORT=YES
export TAN_NODE=$(hostname -f)
export test_exe=${ONLINEKERNELROOT}/${CMTCONFIG}/test.exe
export gaudi_exe="${GAUDIONLINEROOT}/${CMTCONFIG}/Gaudi.exe"
export gaudi_exe_args="libGaudiOnline.so OnlineTask" 
#export MSGSVC=MessageSvc
#echo Working Directory: $PWD
#${gaudi_exe} ${gaudi_exe_args} -main=../options/MBMinit.opts  -opt=../options/Daemon.opts -msgsvc=${MSGSVC}  &

