#/bin/csh
killall test.exe
killall Gaudi.exe
killall gentest.exe
rm /dev/shm/bm_* /dev/shm/sem.bm_* /dev/shm/TAN* /dev/shm/sem.TAN*

setenv MSGSVC MessageSvc
setenv MSGSVC LHCb::DimMessageSvc

setenv TAN_PORT YES
setenv TAN_NODE $HOSTNAME
setenv test_exe   "$ONLINEKERNELROOT/$CMTCONFIG/test.exe "
setenv gaudi_run  "$GAUDIONLINEROOT/$CMTCONFIG/Gaudi.exe libGaudiOnline.so OnlineStart "
setenv gaudi_exe  "$GAUDIONLINEROOT/$CMTCONFIG/Gaudi.exe libGaudiOnline.so OnlineTask -msgsvc=$MSGSVC -auto "
setenv gaudi_exe2 "$GAUDIONLINEROOT/$CMTCONFIG/Gaudi.exe libGaudiOnline.so OnlineTask -msgsvc=MessageSvc -auto "
setenv MINITERM 'xterm  -ls -132 -geometry 132x12 -title '
setenv BIGTERM  'xterm  -ls -132 -geometry 132x45 -title '
setenv WIDETERM 'xterm  -ls -132 -geometry 160x50 -title '
#
#
$MINITERM MBMInit@${HOST} -e "setenv UTGID MEPInit   ; $gaudi_exe -main=$GAUDIONLINEROOT/options/MEPInit.opts -opt=$GAUDIONLINEROOT/options/Daemon.opts " &
#
#
# $MINITERM OutputBuffer@${HOST} -e "setenv UTGID OutBuff; $test_exe mbm_install -s=8096 -e=64 -u=64 -f -i=OUTPUT" &
$MINITERM OutputBuffer@${HOST} -e "setenv UTGID OutBuff   ; $gaudi_exe -main=$GAUDIONLINEROOT/options/MBMinit.opts -opt=$GAUDIONLINEROOT/options/Daemon.opts " &
#
#
##$BIGTERM ErrorLogger@${HOST}    -e "setenv UTGID ErrLog_0; $gaudi_exe -opts=$GAUDIUPIROOT/options/Errlog.opts" &
$WIDETERM ErrorLogger@${HOST}    -e "setenv UTGID ErrLog; $gaudi_exe2 -opts=$GAUDIONLINEROOT/options/ErrorLogger.opts -main=$GAUDIONLINEROOT/options/Remap.opts " &
sleep 3
$MINITERM ErrServ@${HOST} -e "setenv UTGID ErrServ   ; $gaudi_exe -main=$GAUDIONLINEROOT/options/ErrorSrv.opts -opt=$GAUDIONLINEROOT/options/Daemon.opts " &
#
sleep 2
#
#  Monitors:
#
$BIGTERM MBMMon@${HOST} -e "setenv UTGID MBMMon; $gaudi_run libOnlineKernel.so mbm_mon" &
$BIGTERM TANMon@${HOST} -e "setenv UTGID TANMon; $gaudi_run libOnlineKernel.so tanmon -c" &
#
sleep 2
$MINITERM EvtProd@${HOST}   -e "setenv UTGID EvtProd  ; $gaudi_exe -opt=$GAUDIONLINEROOT/options/MEPConverter.opts -main=$GAUDIONLINEROOT/options/Main.opts "&
$MINITERM EvtHolder@${HOST} -e "setenv UTGID EvtHolder; $gaudi_exe -opt=$GAUDIONLINEROOT/options/MEPHolder.opts -main=$GAUDIONLINEROOT/options/Main.opts "&
$MINITERM Moore_0@${HOST}   -e "setenv UTGID Moore_0  ; $gaudi_exe -opt=$GAUDIONLINEROOT/options/ReadMBM.opts -main=$GAUDIONLINEROOT/options/Main.opts "&
$MINITERM Moore_1@${HOST}   -e "setenv UTGID Moore_1  ; $gaudi_exe -opt=$GAUDIONLINEROOT/options/ReadMBM.opts -main=$GAUDIONLINEROOT/options/Main.opts "&
$MINITERM Sender@${HOST}    -e "setenv UTGID Sender   ; $gaudi_exe -opt=$GAUDIONLINEROOT/options/MDFSender.opts -main=$GAUDIONLINEROOT/options/Main.opts "&
#
$MINITERM Receiver@${HOST}  -e "setenv UTGID Receiver ; $gaudi_exe -opt=$GAUDIONLINEROOT/options/MDFReceiver.opts -main=$GAUDIONLINEROOT/options/Main.opts "&
$MINITERM DiskWR@${HOST}    -e "setenv UTGID DiskWR   ; $gaudi_exe -opt=$GAUDIONLINEROOT/options/DiskWR.opts -main=$GAUDIONLINEROOT/options/Main.opts "&
#
#
# For debugging enable this and disable any other
#$MINITERM Sender@${HOST}    -e "setenv UTGID DbgTask   ; cat gaudi.gdb; gdb -x gaudi.gdb $GAUDIONLINEROOT/$CMTCONFIG/Gaudi.exe" &
# $BIGTERM MBMDump@${HOST} -e "setenv UTGID MBMDump; $gaudi_run libMBMDump.so mbmdump" &
# setenv UTGID prod_0; $gaudi_run libGaudiOnline.so mep_producer -n=prod_0 -p=333 -s=500 -r=2
tail -n 3 hlt.csh
