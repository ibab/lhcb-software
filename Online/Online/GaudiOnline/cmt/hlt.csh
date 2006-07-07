#/bin/csh
killall test.exe
killall Gaudi.exe
# rm /dev/shm/bm_* /dev/shm/sem.bm_* /dev/shm/TAN* /dev/shm/sem.TAN*

setenv TAN_PORT YES
setenv TAN_NODE $HOSTNAME
setenv test_exe  "$ONLINEKERNELROOT/$CMTCONFIG/test.exe "
setenv gaudi_exe "$GAUDIONLINEROOT/$CMTCONFIG/Gaudi.exe libGaudiOnline.so OnlineTask"
setenv MSGSVC LHCb::DimMessageSvc
setenv BIGTERM 'xterm  -ls -132 -geometry 132x45'
xterm  -title MBMInit@${HOST} -ls -132 -geometry 132x45 -e "setenv UTGID MBMInit   ; $gaudi_exe $GAUDIONLINEROOT/$CMTCONFIG/libGaudiOnline.so OnlineTask -main=$GAUDIONLINEROOT/options/MBMinit.opts  -opt=$GAUDIONLINEROOT/options/Daemon.opts -msgsvc=$MSGSVC -auto" &
xterm  -title ErrServ@${HOST} -ls -132 -geometry 132x45 -e "setenv UTGID ErrServ   ; $gaudi_exe $GAUDIONLINEROOT/$CMTCONFIG/libGaudiOnline.so OnlineTask -opt=$GAUDIONLINEROOT/options/ErrorSrv.opts -msgsvc=$MSGSVC -auto" &
sleep 2
$BIGTERM -title ErrorLogger@${HOST} -e "setenv UTGID ErrorLogger; cd $GAUDIUPIROOT/cmt; /bin/sh $UPIROOT/cmt/upi.sh $gaudi_exe -opts=$GAUDIUPIROOT/options/Errlog.opts -msgsvc=$MSGSVC -auto" &
sleep 2
#
#  Monitors:
#
$BIGTERM -title MBMMon@${HOST} -e "setenv UTGID MBMMon    ; $test_exe mbm_mon" &
$BIGTERM -title TANMon@${HOST} -e "setenv UTGID TANMon ; $test_exe tanmon -c" &
sleep 4
$BIGTERM -e "setenv UTGID EvtProd   ; $gaudi_exe -opt=$GAUDIONLINEROOT/options/MEPConverter.opts -msgsvc=$MSGSVC -auto"&
$BIGTERM -e "setenv UTGID EvtHolder ; $gaudi_exe -opt=$GAUDIONLINEROOT/options/MEPHolder.opts    -msgsvc=$MSGSVC -auto"&
$BIGTERM -e "setenv UTGID Moore_0   ; $gaudi_exe -opt=$GAUDIONLINEROOT/options/ReadMBM.opts      -msgsvc=$MSGSVC -auto"&
$BIGTERM -e "setenv UTGID Moore_1   ; $gaudi_exe -opt=$GAUDIONLINEROOT/options/ReadMBM.opts      -msgsvc=$MSGSVC -auto"&
#$BIGTERM -e "setenv UTGID Sender   ; $gaudi_exe -opt=$GAUDIONLINEROOT/options/FragmentSender.opts  -msgsvc=$MSGSVC -auto"&
#
# setenv UTGID prod_0; $GAUDIONLINEROOT/$CMTCONFIG/Gaudi.exe libGaudiOnline.so mep_producer -n=prod_0 -p=333 -s=500 -r=2
