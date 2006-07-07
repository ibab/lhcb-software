#/bin/csh
killall test.exe
killall Gaudi.exe
killall gentest.exe
# rm /dev/shm/bm_* /dev/shm/sem.bm_* /dev/shm/TAN* /dev/shm/sem.TAN*

setenv TAN_PORT YES
setenv TAN_NODE $HOSTNAME
setenv test_exe  "$ONLINEKERNELROOT/$CMTCONFIG/test.exe "
setenv gaudi_exe "$GAUDIONLINEROOT/$CMTCONFIG/Gaudi.exe libGaudiOnline.so OnlineTask"
setenv MSGSVC LHCb::DimMessageSvc
setenv MINITERM 'xterm  -ls -132 -geometry 132x12 -title '
setenv BIGTERM  'xterm  -ls -132 -geometry 132x45 -title '
setenv WIDETERM 'xterm  -ls -132 -geometry 160x50 -title '
$MINITERM MBMInit@${HOST} -e "setenv UTGID MBMInit   ; $gaudi_exe $GAUDIONLINEROOT/$CMTCONFIG/libGaudiOnline.so OnlineTask -main=$GAUDIONLINEROOT/options/MBMinit.opts  -opt=$GAUDIONLINEROOT/options/Daemon.opts -msgsvc=$MSGSVC -auto" &
$MINITERM ErrServ@${HOST} -e "setenv UTGID ErrServ   ; $gaudi_exe $GAUDIONLINEROOT/$CMTCONFIG/libGaudiOnline.so OnlineTask -opt=$GAUDIONLINEROOT/options/ErrorSrv.opts -msgsvc=$MSGSVC -auto" &
sleep 2
# $BIGTERM ErrorLogger@${HOST} -e "setenv UTGID ErrLog_0; cd $GAUDIUPIROOT/cmt; /bin/sh $UPIROOT/cmt/upi.sh $gaudi_exe -opts=$GAUDIUPIROOT/options/Errlog.opts -msgsvc=$MSGSVC -auto" &
$MINITERM ErrorLogger@${HOST} -e "setenv UTGID ErrLog_0; $gaudi_exe -opts=$GAUDIUPIROOT/options/Errlog.opts -msgsvc=$MSGSVC -auto" &
#
sleep 2
#
# setenv UTGID ErrDisp_0; ddd --command=aaa $ONLINEKERNELROOT/$CMTCONFIG/gentest.exe &
#
#  Monitors:
#
$BIGTERM MBMMon@${HOST} -e "setenv UTGID MBMMon; $test_exe mbm_mon" &
$BIGTERM TANMon@${HOST} -e "setenv UTGID TANMon; $test_exe tanmon -c" &
sleep 2
$WIDETERM ErrorDiplay@${HOST} -e "setenv UTGID ErrDisp_0; cd $GAUDIUPIROOT/cmt; /bin/sh $UPIROOT/cmt/upi.sh $ONLINEKERNELROOT/$CMTCONFIG/gentest.exe $UPIROOT/slc3_ia32_gcc323_dbg/libUPI.so upi_server -nostamps -c=Errlog_0" &
#
sleep 2
$MINITERM EvtProd@${HOST} -e "setenv UTGID EvtProd   ; $gaudi_exe -opt=$GAUDIONLINEROOT/options/MEPConverter.opts -msgsvc=$MSGSVC -auto"&
$MINITERM EvtHolder@${HOST} -e "setenv UTGID EvtHolder ; $gaudi_exe -opt=$GAUDIONLINEROOT/options/MEPHolder.opts    -msgsvc=$MSGSVC -auto"&
$MINITERM Moore_0@${HOST} -e "setenv UTGID Moore_0   ; $gaudi_exe -opt=$GAUDIONLINEROOT/options/ReadMBM.opts      -msgsvc=$MSGSVC -auto"&
$MINITERM Moore_1@${HOST} -e "setenv UTGID Moore_1   ; $gaudi_exe -opt=$GAUDIONLINEROOT/options/ReadMBM.opts      -msgsvc=$MSGSVC -auto"&
#$BIGTERM -e "setenv UTGID Sender   ; $gaudi_exe -opt=$GAUDIONLINEROOT/options/FragmentSender.opts  -msgsvc=$MSGSVC -auto"&
#
# setenv UTGID prod_0; $GAUDIONLINEROOT/$CMTCONFIG/Gaudi.exe libGaudiOnline.so mep_producer -n=prod_0 -p=333 -s=500 -r=2
