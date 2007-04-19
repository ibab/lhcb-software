#
setenv UTGID $1
shift
setenv DIM_DNS_NODE pclhcb11.cern.ch


setenv CMTCONFIG slc4_ia32_gcc34

setenv SHLVL 1
source $LHCBHOME/scripts/CMT.csh
setenv CMTPROJECTPATH $LHCBPROJECTPATH


source /afs/cern.ch/user/e/evh/w0/newcmt/Online_v3r1/Online/OnlineTasks/cmt/setup.csh

setenv GAUDIONLINEROOT /afs/cern.ch/lhcb/software/releases/ONLINE/ONLINE_v3r1/Online/GaudiOnline/v2r6

setenv TAN_PORT YES
setenv TAN_NODE `hostname -f`
setenv  test_exe $GAUDIONLINEROOT/$CMTCONFIG/test.exe
setenv  gaudi_exe $GAUDIONLINEROOT/$CMTCONFIG/Gaudi.exe
setenv  gaudi_exe_args "$GAUDIONLINEROOT/$CMTCONFIG/libGaudiOnline.so OnlineTask" 
setenv MSGSVC LHCb::DimMessageSvc

$gaudi_exe $gaudi_exe_args -main=$GAUDIONLINEROOT/options/ErrorSrv.opts -opt=$GAUDIONLINEROOT/options/Daemon.opts
#    -msgsvc=${MSGSVC} &
