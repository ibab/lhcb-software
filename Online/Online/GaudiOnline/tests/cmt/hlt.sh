#/bin/csh
#killall test.exe
killall Gaudi.exe
killall gentest.exe
#rm /dev/shm/bm_* /dev/shm/sem.bm_* /dev/shm/TAN* /dev/shm/sem.TAN*

export MSGSVC=LHCb::FmcMessageSvc
export MSGSVC=MessageSvc

export TAN_PORT=YES
export TAN_NODE=$HOSTNAME
export OPTS=$GAUDIONLINEROOT/options
export test_exe="$ONLINEKERNELROOT/$CMTCONFIG/test.exe "
export gaudi_run="$GAUDIONLINEROOT/$CMTCONFIG/Gaudi.exe libGaudiOnline.so OnlineStart "
export gaudi_exe="$GAUDIONLINEROOT/$CMTCONFIG/Gaudi.exe libGaudiOnline.so OnlineTask -msgsvc=$MSGSVC -auto "
export gaudi_exe2="$GAUDIONLINEROOT/$CMTCONFIG/Gaudi.exe libGaudiOnline.so OnlineTask -msgsvc=MessageSvc -auto "
export MINITERM='xterm  -ls -132 -geometry 132x12 -title '
export BIGTERM='xterm  -ls -132 -geometry 132x45 -title '
export WIDETERM='xterm  -ls -132 -geometry 160x50 -title '
#
#
$MINITERM MBMInit@${HOST} -e "export UTGID=MEPInit   ; $gaudi_exe -opts=$OPTS/MEPInit.opts -main=$OPTS/Main.opts " &
$MINITERM OutputBuffer@${HOST} -e "export UTGID=OutBuff   ; $gaudi_exe -opts=$OPTS/MBMinit.opts -main=$OPTS/Main.opts " &
#
#
##$BIGTERM ErrorLogger@${HOST}    -e "export UTGID=ErrLog_0; $gaudi_exe -opts=$GAUDIUPIROOT/options/Errlog.opts" &
#$WIDETERM ErrorLogger@${HOST}    -e "export UTGID=ErrLog; $gaudi_exe2 -opts=$OPTS/ErrorLogger.opts -main=$OPTS/Remap.opts " &
#sleep 3
#$MINITERM ErrSrv@${HOST} -e "export UTGID=ErrSrv   ; $gaudi_exe -main=$OPTS/ErrorSrv.opts -opt=$OPTS/Daemon.opts " &
#
sleep 4
#
#  Monitors:
#
$BIGTERM MBMMon@${HOST} -e "export UTGID=MBMMon; $gaudi_run libOnlineKernel.so mbm_mon" &
#$BIGTERM MBMMon@${HOST} -e "export UTGID=MBMMon; $test_exe mbm_mon" &
$BIGTERM TANMon@${HOST} -e "export UTGID=TANMon; $gaudi_run libOnlineKernel.so tanmon -c" &
#
$MINITERM EvtProd@${HOST}   -e "export UTGID=EvtProd;   $gaudi_exe -opt=$OPTS/MEPConverter.opts -main=$OPTS/Main.opts "&
$MINITERM EvtHolder@${HOST} -e "export UTGID=EvtHolder; $gaudi_exe -opt=$OPTS/MEPHolder.opts -main=$OPTS/Main.opts "&
$MINITERM Moore_0@${HOST}   -e "export UTGID=Moore_0;   $gaudi_exe -opt=$OPTS/ReadMBM.opts -main=$OPTS/Main.opts "&
$MINITERM Moore_1@${HOST}   -e "export UTGID=Moore_1;   $gaudi_exe -opt=$OPTS/ReadMBM.opts -main=$OPTS/Main.opts "&
$MINITERM Moore_2@${HOST}   -e "export UTGID=Moore_2;   $gaudi_exe -opt=$OPTS/ReadMBM.opts -main=$OPTS/Main.opts "&
# $MINITERM Moore_3@${HOST} -e "export UTGID=Moore_3;   $gaudi_exe -opt=$OPTS/ReadMBM.opts -main=$OPTS/Main.opts "&
$MINITERM Receiver@${HOST}  -e "export UTGID=Receiver;  $gaudi_exe -opt=$OPTS/MDFReceiver.opts -main=$OPTS/Main.opts "&
# $MINITERM DiskWR@${HOST}  -e "export UTGID=DiskWR;    $gaudi_exe -opt=$OPTS/DiskWR.opts -main=$OPTS/Main.opts "&
$MINITERM EvtServ@${HOST}   -e "export UTGID=EvtServ;   $gaudi_exe -opt=$OPTS/EventServer.opts -main=$OPTS/Main.opts "&

sleep 2

$MINITERM Sender@${HOST}    -e "export UTGID=Sender   ; $gaudi_exe -opt=$OPTS/MDFSender.opts -main=$OPTS/Main.opts "&
$MINITERM Sender2@${HOST}    -e "export UTGID=Sender2   ; $gaudi_exe -opt=$OPTS/MDFSender.opts -main=$OPTS/Main.opts "&

#sleep 2
$MINITERM EvtCons1@${HOST}    -e "export UTGID=EvtCons1;  $gaudi_exe -opt=$OPTS/NetworkConsumer.opts -main=$OPTS/Main.opts "&
$MINITERM EvtCons2@${HOST}    -e "export UTGID=EvtCons2;  $gaudi_exe -opt=$OPTS/NetworkConsumer.opts -main=$OPTS/Main.opts "&
#
# For debugging enable this and disable any other
#$MINITERM Sender@${HOST}    -e "export UTGID=DbgTask   ; cat gaudi.gdb; gdb -x gaudi.gdb $GAUDIONLINEROOT/$CMTCONFIG/Gaudi.exe" &
# $BIGTERM MBMDump@${HOST} -e "export UTGID=MBMDump; $gaudi_run libMBMDump.so mbmdump" &
# export UTGID=prod_0; $gaudi_run libGaudiOnline.so mep_producer -n=prod_0 -p=333 -s=500 -r=2
tail -n 3 hlt.sh
