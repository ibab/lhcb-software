# 
# hltrx.sh
# Markus Frank & Niko Neufeld
# Simple script to run the event builder chain stand-alone
# Requires CMTPROJECTPATH set and setup.sh sourced in $ONLINETASKSROOT/cmt
# 
#
if [ -z DIM_DNS_NODE ]; then
	echo "Please define DIM_DNS_NODE"
	exit 1
fi
# Cleanup any running jobs
pkill Gaudi.exe
pkill test.exe
pkill gentest.exe
rm -f /dev/shm/bm_*  /dev/shm/sem.bm_* /dev/shm/TAN* /dev/shm/sem.TAN*

# message are send via the DIM Messageservice
export MSGSVC=MessageSvc
export MSGSVC=LHCb::DimMessageSvc

# TAN - do not touch unless you know what you're doing
export TAN_PORT YES
export TAN_NODE=$(hostname -f)

export BIGTERM='xterm  -ls -132 -geometry 132x45 -title ' 
export WIDETERM='xterm  -sl 10000 -ls -132 -geometry 160x50 -title '
export MINITERM='xterm -sl 100000 -ls -132 -geometry 132x10 -title '

# shortcuts for starting jobs
export test_exe=${ONLINEKERNELROOT}/${CMTCONFIG}/test.exe
export gaudi_run="${GAUDIONLINEROOT}/${CMTCONFIG}/Gaudi.exe libGaudiOnline.so OnlineStart "
export gaudi_exe="${GAUDIONLINEROOT}/${CMTCONFIG}/Gaudi.exe libGaudiOnline.so OnlineTask -msgsvc=${MSGSVC} -auto"  
export gaudi_exe2="${GAUDIONLINEROOT}/${CMTCONFIG}/Gaudi.exe libGaudiOnline.so OnlineTask -msgsvc=MessageSvc -auto"  

# Online tasks - be careful when changing the startup order 
 
# MBMInit initializes the shared memory
$MINITERM MEPInit@${HOST} -e "export UTGID=MEPInit ; ${gaudi_exe} -main=${ONLINETASKSROOT}/options/MEPInit.opts  -opt=$GAUDIONLINEROOT/options/Daemon.opts " &
#
# ErrorLogger (not required for running)
$WIDETERM ErrorLogger@${HOST}    -e "export UTGID=ErrLog; ${gaudi_exe2} -opts=$GAUDIONLINEROOT/options/ErrorLogger.opts -main=$GAUDIONLINEROOT/options/Remap.opts " &
#
$MINITERM ErrServ@${HOST} -e "export UTGID=ErrServ ; ${gaudi_exe} -main=$GAUDIONLINEROOT/options/ErrorSrv.opts -opt=$GAUDIONLINEROOT/options/Daemon.opts " &
#
sleep 2
#
#  Monitors:
#
$BIGTERM MBMMon@${HOST} -e "export UTGID=MBMMon ; ${gaudi_run} libOnlineKernel.so mbm_mon" &
#
sleep 2

# The following 2 processes are essential
$MINITERM EvtProd@${HOST}   -e "export UTGID=EvtProd  ; ${gaudi_exe} -opt=$GAUDIONLINEROOT/options/MEPConverter.opts -main=$GAUDIONLINEROOT/options/Main.opts "&
$MINITERM EvtHolder@${HOST} -e "export UTGID=EvtHolder ; ${gaudi_exe} -opt=$GAUDIONLINEROOT/options/MEPHolder.opts -main=$GAUDIONLINEROOT/options/Main.opts "&
#
# At least one Consumer is needed - for example the (dummy) Moore process
#
$MINITERM Moore_0@${HOST}   -e "export UTGID=Moore_0  ; ${gaudi_exe} -opt=$GAUDIONLINEROOT/options/ReadMBM.opts -main=$GAUDIONLINEROOT/options/Main.opts "&
$MINITERM Moore_1@${HOST}   -e "export UTGID=Moore_1  ; ${gaudi_exe} -opt=$GAUDIONLINEROOT/options/ReadMBM.opts -main=$GAUDIONLINEROOT/options/Main.opts "&
#$MINITERM Moore_2@${HOST}   -e "export UTGID=Moore_2  ; $gaudi_exe -opt=$GAUDIONLINEROOT/options/ReadMBM.opts -main=$GAUDIONLINEROOT/options/Main.opts "&
# $MINITERM Moore_3@${HOST}   -e "export UTGID= Moore_3  ; $gaudi_exe -opt=$GAUDIONLINEROOT/options/ReadMBM.opts -main=$GAUDIONLINEROOT/options/Main.opts "&
#
# Optional if you want write the data
#
#$MINITERM DiskWR@${HOST}    -e "export UTGID=DiskWR   ; ${gaudi_exe} -opt=$ONLINETASKSROOT/options/DiskWR.opts -main=$GAUDIONLINEROOT/options/Main.opts "&
#
# Last not least the event-builder
#
$BIGTERM MEPRx@${HOST} -e "export UTGID=MEPRx ; ${gaudi_exe} -opt=$ONLINETASKSROOT/options/MEPRxSvc.opts -main=$GAUDIONLINEROOT/options/Main.opts "&
