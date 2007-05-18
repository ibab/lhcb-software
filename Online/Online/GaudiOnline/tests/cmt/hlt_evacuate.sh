. $GAUDIONLINEROOT/tests/cmt/preamble.sh

killall DNS.exe
$DIMROOT/$CMTCONFIG/DNS.exe &
$MINITERM OutputBuffer@${HOST} -e "/bin/sh -c 'export UTGID=OutBuff;$gaudi_exe -main=$OPTS/MBMinit.opts -opt=$OPTS/Daemon.opts'" &
$MINITERM InputBuffer@${HOST} -e "/bin/sh -c 'export UTGID=InputBuffer;$gaudi_exe -main=$OPTS/MBMfarm.opts -opt=$OPTS/Daemon.opts'" &
#
$WIDETERM ErrorLogger@${HOST}    -e "setenv UTGID ErrLog; $gaudi_exe2 -opts=$GAUDIONLINEROOT/options/ErrorLogger.opts -main=$GAUDIONLINEROOT/options/Remap.opts " &
sleep 3
$MINITERM ErrSrv@${HOST} -e "setenv UTGID ErrSrv   ; $gaudi_exe -main=$GAUDIONLINEROOT/options/ErrorSrv.opts -opt=$GAUDIONLINEROOT/options/Daemon.opts " &
#
sleep 8
#
$BIGTERM MBMMon@${HOST} -e "/bin/sh -c 'export UTGID=MBMMon;$gaudi_run libOnlineKernel.so mbm_mon'" &
$BIGTERM TANMon@${HOST} -e "/bin/sh -c 'export UTGID=TANMon;$gaudi_run libOnlineKernel.so tanmon -c'" &
#
export UTGID=RECV_0
$MINITERM Recv_0@${HOST} -e "/bin/sh -c '$gaudi_exe -opt=$OPTS/NetReceiver.opts -main=$GAUDIONLINEROOT/options/Main.opts -auto'" &
#ddd --command=gaudi.gdb $GAUDIONLINEROOT/$CMTCONFIG/Gaudi.exe &
sleep 5
export UTGID=SEND0_0
$MINITERM Send0_0@${HOST} -e "/bin/sh -c '$gaudi_exe -opt=$OPTS/NetSend_0.opts -main=$GAUDIONLINEROOT/options/Main.opts -auto'" &
# $gaudi_exe -opt=$OPTS/NetSend_0.opts -main=$GAUDIONLINEROOT/options/Main.opts -auto &
# ddd --command=./gaudi.gdb $GAUDIONLINEROOT/$CMTCONFIG/Gaudi.exe &
sleep 1
export UTGID=Prod_0
# time $gaudi_exe -opt=$OPTS/MDF2MBM.opts  -msgsvc=$msg_svc
# $GAUDIONLINEROOT/$CMTCONFIG/Gaudi.exe libGaudiOnline.so mdf_producer -n=prod_0 -p=333 -s=500 -b=OUT_14d -f=./mdfOutput.dat
/usr/bin/tail -n 3 ./hlt_evacuate.sh
