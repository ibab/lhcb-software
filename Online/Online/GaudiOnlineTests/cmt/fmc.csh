#/bin/csh
#killall DNS.exe
killall test.exe
killall Gaudi.exe
killall gentest.exe
rm /dev/shm/bm_* /dev/shm/sem.bm_* /dev/shm/TAN* /dev/shm/sem.TAN*

setenv MSGSVC MessageSvc
setenv MSGSVC LHCb::FmcMessageSvc

setenv TAN_PORT YES
setenv TAN_NODE $HOSTNAME
setenv gaudi_run  "$GAUDIONLINEROOT/$CMTCONFIG/Gaudi.exe libGaudiOnline.so OnlineStart "
setenv gaudi_exe  "$GAUDIONLINEROOT/$CMTCONFIG/Gaudi.exe libGaudiOnline.so OnlineTask -msgsvc=$MSGSVC -auto "
setenv MINITERM 'xterm  -ls -132 -geometry 132x12 -title '
setenv BIGTERM  'xterm  -ls -132 -geometry 132x45 -title '
setenv DIM_DNS_NODE $HOSTNAME
#../../../DIM/$CMTCONFIG/DNS.exe &
#
$MINITERM MBMInit@${HOST} -e "setenv UTGID MEPInit   ; $gaudi_exe -main=$GAUDIONLINEROOT/options/MEPInit.opts -opt=$GAUDIONLINEROOT/options/Daemon.opts" &
#
sleep 2
$BIGTERM MBMMon@${HOST} -e "setenv UTGID MBMMon; $gaudi_run libOnlineKernel.so mbm_mon" &
#
sleep 4
$MINITERM EvtProd@${HOST}   -e "setenv UTGID EvtProd  ; $gaudi_exe -opt=$GAUDIONLINEROOT/options/MEPConverter.opts"&
$MINITERM EvtHolder@${HOST} -e "setenv UTGID EvtHolder; $gaudi_exe -opt=$GAUDIONLINEROOT/options/MEPHolder.opts"&
#
# For debugging enable this and disable any other
# setenv UTGID prod_0; $gaudi_run libGaudiOnline.so mep_producer -n=prod_0 -p=333 -s=500 -r=2
tail -n 3 hlt.csh
