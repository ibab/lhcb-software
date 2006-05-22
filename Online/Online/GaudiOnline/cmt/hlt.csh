#!/bin/tcsh
rm /dev/shm/bm_* /dev/shm/sem.bm_* /dev/shm/TAN* /dev/shm/sem.TAN*
rm -f /dev/shm/*
sudo killall -9 test.exe
sudo killall -9 Gaudi.exe
setenv TAN_PORT YES
setenv TAN_NODE $HOSTNAME
setenv test_exe "$ONLINEKERNELROOT/$CMTCONFIG/test.exe "
setenv gaudi_exe "$GAUDIONLINEROOT/$CMTCONFIG/Gaudi.exe "
setenv MSGSVC MessageSvc
setenv SHELL tcsh
#setenv UTGID MBMMon    ; %test_exe% mbm_mon
xterm  -ls -132 -geometry 132x45 -e "setenv UTGID MBMInit   ; $gaudi_exe ../$CMTCONFIG/libGaudiOnline.so OnlineTask -main=../options/MBMinit.opts  -opt=../options/Daemon.opts -msgsvc=MessageSvc -a" &
#setenv UTGID TANServ   ; $gaudi_exe -main=../options/TanAlloc.opts ../$CMTCONFIG/libGaudiOnline.so OnlineTask -opt=../options/Daemon.opts -msgsvc=MessageSvc -a
#setenv UTGID TANMon    ; "Tanmon"  %test_exe% tanmon -c
xterm  -ls -132 -geometry 132x45 -e "setenv UTGID ErrServ   ; $gaudi_exe ../$CMTCONFIG/libGaudiOnline.so OnlineTask -opt=../options/ErrorSrv.opts -msgsvc=MessageSvc -a" &
sleep 8
xterm  -ls -132 -geometry 132x45 -e "setenv UTGID EvtProd   ; $gaudi_exe ../$CMTCONFIG/libGaudiOnline.so OnlineTask -opt=../options/MEPConverter.opts -msgsvc=MessageSvc -a"&
xterm  -ls -132 -geometry 132x45 -e "setenv UTGID EvtHolder ; $gaudi_exe ../$CMTCONFIG/libGaudiOnline.so OnlineTask -opt=../options/MEPHolder.opts    -msgsvc=MessageSvc -a"&
#xterm  -ls -132 -geometry 132x45 -e "setenv UTGID Moore_0   ; $gaudi_exe ../$CMTCONFIG/libGaudiOnline.so OnlineTask -opt=../options/ReadMBM.opts      -msgsvc=MessageSvc -a"&
#xterm  -ls -132 -geometry 132x45 -e "setenv UTGID Moore_1   ; $gaudi_exe ../$CMTCONFIG/libGaudiOnline.so OnlineTask -opt=../options/ReadMBM.opts      -msgsvc=MessageSvc -a"&
#xterm  -ls -132 -geometry 132x45 -e "setenv UTGID Sender    ; $gaudi_exe ../$CMTCONFIG/libGaudiOnline.so OnlineTask -opt=../options/FragmentSender.opts  -msgsvc=MessageSvc -a"&
xterm  -ls -132 -geometry 132x40 -e "$test_exe mbm_mon" &

sleep 2
setenv UTGID prod_0; ../$CMTCONFIG/Gaudi.exe libGaudiOnline.so mep_producer -n=prod_0 -p=333 -s=500 -r=2
