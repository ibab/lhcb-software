# 
# hltrx.sh
# Markus Frank & Niko Neufeld
# Simple script to run the event builder chain stand-alone
# Requires CMTPROJECTPATH set and setup.sh sourced in $ONLINETASKSROOT/cmt
# 
#

# Cleanup any running jobs
killall -q Gaudi.exe
killall -q test.exe
killall -q gentest.exe

# message are send via the DIM Messageservice
setenv MSGSVC MessageSvc
setenv MSGSVC LHCb::DimMessageSvc

# TAN - do not touch unless you know what you're doing
setenv TAN_PORT YES
setenv TAN_NODE `hostname -f`

setenv BIGTERM 'xterm  -ls -132 -geometry 132x45 -title ' 
setenv WIDETERM 'xterm  -sl 10000 -ls -132 -geometry 160x50 -title '
setenv MINITERM 'xterm -sl 100000 -ls -132 -geometry 132x10 -title '

# shortcuts for starting jobs
setenv test_exe ${ONLINEKERNELROOT}/${CMTCONFIG}/test.exe
setenv gaudi_run "${GAUDIONLINEROOT}/${CMTCONFIG}/Gaudi.exe libGaudiOnline.so OnlineStart "
setenv gaudi_exe "${GAUDIONLINEROOT}/${CMTCONFIG}/Gaudi.exe libGaudiOnline.so OnlineTask -msgsvc=${MSGSVC} -auto"  
setenv gaudi_exe2 "${GAUDIONLINEROOT}/${CMTCONFIG}/Gaudi.exe libGaudiOnline.so OnlineTask -msgsvc=MessageSvc -auto"  

# Online tasks - be careful when changing the startup order 
 
# MBMInit initializes the shared memory
$MINITERM MEPInit@${HOST} -e "setenv UTGID MEPInit ; ${gaudi_exe} -main=${GAUDIONLINEROOT}/options/MEPInit.opts  -opt=$GAUDIONLINEROOT/options/Daemon.opts " &
#
$MINITERM OutputBuffer@${HOST} -e "setenv UTGID OutBuff ; ${gaudi_exe} -main=$GAUDIONLINEROOT/options/MBMinit.opts -opt=$GAUDIONLINEROOT/options/Daemon.opts " &
# ErrorLogger (not required for running)
$WIDETERM ErrorLogger@${HOST}    -e "setenv UTGID ErrLog; ${gaudi_exe2} -opts=$GAUDIONLINEROOT/options/ErrorLogger.opts -main=$GAUDIONLINEROOT/options/Remap.opts " &
#
$MINITERM ErrServ@${HOST} -e "setenv UTGID ErrServ ; ${gaudi_exe} -main=$GAUDIONLINEROOT/options/ErrorSrv.opts -opt=$GAUDIONLINEROOT/options/Daemon.opts " &
#
sleep 2
#
#  Monitors:
#
$BIGTERM MBMMon@${HOST} -e "setenv UTGID MBMMon ; ${gaudi_run} libOnlineKernel.so mbm_mon" &
#
sleep 2

# The following 2 processes are essential
$MINITERM EvtProd@${HOST}   -e "setenv UTGID EvtProd  ; ${gaudi_exe} -opt=$GAUDIONLINEROOT/options/MEPConverter.opts -main=$GAUDIONLINEROOT/options/Main.opts "&
$MINITERM EvtHolder@${HOST} -e "setenv UTGID EvtHolder ; ${gaudi_exe} -opt=$GAUDIONLINEROOT/options/MEPHolder.opts -main=$GAUDIONLINEROOT/options/Main.opts "&
#
# At least one Consumer is needed - for example the (dummy) Moore process
#
$MINITERM Moore_0@${HOST}   -e "setenv UTGID Moore_0  ; ${gaudi_exe} -opt=$GAUDIONLINEROOT/options/ReadMBM.opts -main=$GAUDIONLINEROOT/options/Main.opts "&
$MINITERM Moore_1@${HOST}   -e "setenv UTGID Moore_1  ; ${gaudi_exe} -opt=$GAUDIONLINEROOT/options/ReadMBM.opts -main=$GAUDIONLINEROOT/options/Main.opts "&
#$MINITERM Moore_2@${HOST}   -e "setenv UTGID Moore_2  ; $gaudi_exe -opt=$GAUDIONLINEROOT/options/ReadMBM.opts -main=$GAUDIONLINEROOT/options/Main.opts "&
# $MINITERM Moore_3@${HOST}   -e "setenv UTGID  Moore_3  ; $gaudi_exe -opt=$GAUDIONLINEROOT/options/ReadMBM.opts -main=$GAUDIONLINEROOT/options/Main.opts "&
#
# Optional if you want write the data
#
$MINITERM DiskWR@${HOST}    -e "setenv UTGID DiskWR   ; ${gaudi_exe} -opt=$ONLINETASKSROOT/options/DiskWR.opts -main=$GAUDIONLINEROOT/options/Main.opts "&
#
# Last not least the event-builder
#
$BIGTERM MEPRx@${HOST} -e "setenv UTGID MEPRx ; ${gaudi_exe} -opt=$ONLINETASKSROOT/options/MEPRxSvc.opts -main=$GAUDIONLINEROOT/options/Main.opts "&

#$MINITERM MDFWriter@${HOST} -e "setenv UTGID MDFWriter ; ${gaudi_exe} -opt=$ONLINETASKSROOT/options/MDFWriterLite.opts -main=$GAUDIONLINEROOT/options/Main.opts " &
