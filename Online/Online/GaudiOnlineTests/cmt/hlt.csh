#/bin/csh
killall test.exe
killall Gaudi.exe
killall gentest.exe
rm /dev/shm/bm_* /dev/shm/sem.bm_* /dev/shm/TAN* /dev/shm/sem.TAN*

setenv MSGSVC LHCb::DimMessageSvc
setenv MSGSVC MessageSvc

setenv TAN_PORT YES
setenv TAN_NODE $HOSTNAME
setenv OPTS        $GAUDIONLINEROOT/options
setenv test_exe   "$ONLINEKERNELROOT/$CMTCONFIG/test.exe "
setenv gaudi_run  "$GAUDIONLINEROOT/$CMTCONFIG/Gaudi.exe libGaudiOnline.so OnlineStart "
setenv gaudi_exe  "$GAUDIONLINEROOT/$CMTCONFIG/Gaudi.exe libGaudiOnline.so OnlineTask -msgsvc=$MSGSVC -auto "
setenv gaudi_exe2 "$GAUDIONLINEROOT/$CMTCONFIG/Gaudi.exe libGaudiOnline.so OnlineTask -msgsvc=MessageSvc -auto "
setenv MINITERM 'xterm  -ls -132 -geometry 132x12 -title '
setenv BIGTERM  'xterm  -ls -132 -geometry 132x45 -title '
setenv WIDETERM 'xterm  -ls -132 -geometry 160x50 -title '
#
#
$MINITERM MBMInit@${HOST} -e "setenv UTGID MEPInit   ; $gaudi_exe -opt=$OPTS/MEPInit.opts -main=$OPTS/Main.opts " &
#
#
# $MINITERM OutputBuffer@${HOST} -e "setenv UTGID OutBuff; $test_exe mbm_install -s=8096 -e=64 -u=64 -f -i=OUTPUT" &
$MINITERM OutputBuffer@${HOST} -e "setenv UTGID OutBuff   ; $gaudi_exe -opt=$OPTS/MBMinit.opts -main=$OPTS/Main.opts " &
#
$MINITERM TanServer@${HOST} -e "setenv UTGID TANServer; $test_exe tan_nameserver -a -tcp -d" &
#
#
##$BIGTERM ErrorLogger@${HOST}    -e "setenv UTGID ErrLog_0; $gaudi_exe -opts=$GAUDIUPIROOT/options/Errlog.opts" &
#$WIDETERM ErrorLogger@${HOST}    -e "setenv UTGID ErrLog; $gaudi_exe2 -opts=$OPTS/ErrorLogger.opts -main=$OPTS/Remap.opts " &
#sleep 3
#$MINITERM ErrSrv@${HOST} -e "setenv UTGID ErrSrv   ; $gaudi_exe -main=$OPTS/ErrorSrv.opts -opt=$OPTS/Daemon.opts " &
#
sleep 2
#
#  Monitors:
#
$BIGTERM MBMMon@${HOST} -e "setenv UTGID MBMMon; $gaudi_run libOnlineKernel.so mbm_mon" &
# $BIGTERM TANMon@${HOST} -e "setenv UTGID TANMon; $gaudi_run libOnlineKernel.so tanmon -c" &
#
sleep 2
$MINITERM EvtProd@${HOST}   -e "setenv UTGID EvtProd  ; $gaudi_exe -opt=$OPTS/MEPConverter.opts -main=$OPTS/Main.opts "&
$MINITERM EvtHolder@${HOST} -e "setenv UTGID EvtHolder; $gaudi_exe -opt=$OPTS/MEPHolder.opts -main=$OPTS/Main.opts "&
$MINITERM Moore_0@${HOST}   -e "setenv UTGID Moore_0  ; $gaudi_exe -opt=$OPTS/ReadMBM.opts -main=$OPTS/Main.opts "&
#$MINITERM Moore_1@${HOST}   -e "setenv UTGID Moore_1  ; $gaudi_exe -opt=$OPTS/ReadMBM.opts -main=$OPTS/Main.opts "&
#$MINITERM Moore_2@${HOST}   -e "setenv UTGID Moore_2  ; $gaudi_exe -opt=$OPTS/ReadMBM.opts -main=$OPTS/Main.opts "&
# $MINITERM Moore_3@${HOST}   -e "setenv UTGID Moore_3  ; $gaudi_exe -opt=$OPTS/ReadMBM.opts -main=$OPTS/Main.opts "&
#
$MINITERM Receiver@${HOST}  -e "setenv UTGID Receiver ; $gaudi_exe -opt=$OPTS/MDFReceiver.opts -main=$OPTS/Main.opts "&
#$MINITERM MDFWriterNet@${HOST}    -e "setenv UTGID MDFWriterNet  ; $gaudi_exe -opt=$OPTS/MDFWriterNet.opts -main=$OPTS/Main.opts "&
#
sleep 2
$MINITERM Sender@${HOST}    -e "setenv UTGID Sender   ; $gaudi_exe -opt=$OPTS/MDFSender.opts -main=$OPTS/Main.opts "&
#
# For debugging enable this and disable any other
# $MINITERM Sender@${HOST}    -e "setenv UTGID DbgTask   ; cat gaudi.gdb; gdb -x gaudi.gdb $GAUDIONLINEROOT/$CMTCONFIG/Gaudi.exe" &
# $BIGTERM MBMDump@${HOST} -e "setenv UTGID MBMDump; $gaudi_run libMBMDump.so mbmdump" &
# setenv UTGID prod_0; $gaudi_run libGaudiOnline.so mep_producer -n=prod_0 -p=333 -s=500 -r=2
tail -n 3 hlt.csh
