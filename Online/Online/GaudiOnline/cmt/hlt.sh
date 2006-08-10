rm -f /dev/shm/*
killall  Gaudi.exe
killall  test.exe
export TAN_PORT=YES
export TAN_NODE=$(hostname -f)
export test_exe=${ONLINEKERNELROOT}/${CMTCONFIG}/test.exe
export gaudi_exe="${GAUDIONLINEROOT}/${CMTCONFIG}/Gaudi.exe libGaudiOnline.so OnlineTask" 
export MSGSVC=MessageSvc
export UTGID=MBMInit   && ${gaudi_exe} -main=../options/MBMinit.opts  -opt=../options/Daemon.opts -msgsvc=${MSGSVC} -auto &
export UTGID=ErrServ   &&  ${gaudi_exe} -opt=../options/ErrorSrv.opts -msgsvc=${MSGSVC} -auto &
sleep 2
export UTGID=EvtHolder &&  ${gaudi_exe} -opt=../options/MEPHolder.opts    -msgsvc=${MSGSVC} -auto &
#export UTGID=Moore_0   &&  ${gaudi_exe} -opt=../options/ReadMBM.opts      -msgsvc=${MSGSVC} -auto &
#export UTGID=Moore_1   &&  ${gaudi_exe} -opt=../options/ReadMBM.opts      -msgsvc=${MSGSVC} -auto &
export UTGID=MBMMon    && xterm -geometry 120x30 -e "${test_exe} mbm_mon" &
sleep 1
echo $# $*
sleep 1

export UTGID=EvtProd   && ${gaudi_exe} -opt=../options/MEPConverter.opts -msgsvc=${MSGSVC} -auto &

if [ $# -eq 1 ] ; then

echo  libGaudiOnline.so OnlineTask -opt=../options/MEPRxSvc.opts -msgsvc=${MSGSVC} -auto &
export UTGID=MEPRx      &&  gdb ${GAUDIONLINEROOT}/${CMTCONFIG}/Gaudi.exe 
else 
export UTGID=MEPRx      &&  ${gaudi_exe} -opt=../options/MEPRxSvc.opts -msgsvc=${MSGSVC} -auto &
fi

echo libGaudiOnline.so OnlineTask  -opt=../options/ReadMBM.opts      -msgsvc=${MSGSVC} -auto

export UTGID=Moore_2 && gdb -x meprx.gdb ${GAUDIONLINEROOT}/${CMTCONFIG}/Gaudi.exe


#export UTGID=Sender    &&  ${gaudi_exe} -opt=../options/FragmentSender.opts  -msgsvc=${MSGSVC} -auto &


 


#  "prod_0" ../slc3_ia32_gcc323_dbg/Gaudi.exe GaudiOnline mep_producer -n=prod_0 -p=333 -s=5
