rm -f /dev/shm/*
killall Gaudi.exe
export TAN_PORT=YES
export TAN_NODE=$(hostname -f)
export test_exe=${ONLINEKERNELROOT}/${CMTCONFIG}/test.exe
export gaudi_exe="${GAUDIONLINEROOT}/${CMTCONFIG}/Gaudi.exe libGaudiOnline.so OnlineTask" 
#export UTGID=MBMMon    && ${test_exe} mbm_mon
export UTGID=MBMInit   && ${gaudi_exe} -main=../options/MBMinit.opts  -opt=../options/Daemon.opts -msgsvc=LHCb::DimMessageSvc -a &
# export UTGID=TANServ   &&  ${gaudi_exe} -main=../options/TanAlloc.opts -opt=../options/Daemon.opts -msgsvc=LHCb::DimMessageSvc -a
# export UTGID=TANMon    &&  "Tanmon"  ${test_exe} tanmon -c
export UTGID=ErrServ   &&  ${gaudi_exe} -opt=../options/ErrorSrv.opts -msgsvc=LHCb::DimMessageSvc -a &
sleep 8
export UTGID=EvtProd   &&  ${gaudi_exe} -opt=../options/MEPConverter.opts -msgsvc=LHCb::DimMessageSvc -a &
export UTGID=EvtHolder &&  ${gaudi_exe} -opt=../options/MEPHolder.opts    -msgsvc=LHCb::DimMessageSvc -a &
export UTGID=Moore_0   &&  ${gaudi_exe} -opt=../options/ReadMBM.opts      -msgsvc=LHCb::DimMessageSvc -a &
#export UTGID=Moore_1   &&  ${gaudi_exe} -opt=../options/ReadMBM.opts      -msgsvc=LHCb::DimMessageSvc -a &
#export UTGID=Sender    &&  ${gaudi_exe} -opt=../options/FragmentSender.opts  -msgsvc=LHCb::DimMessageSvc -a &
#
#  "prod_0" ../slc3_ia32_gcc323_dbg/Gaudi.exe GaudiOnline mep_producer -n=prod_0 -p=333 -s=500 -r=2
