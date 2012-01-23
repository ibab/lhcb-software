# 
# hltrx.sh
# Markus Frank & Niko Neufeld
# Simple script to run the event builder chain stand-alone
# Requires CMTPROJECTPATH set and setup.sh sourced in $ONLINETASKSROOT/cmt
# Does not use EVTPROD anymore for testing only 
#
if [ -z $DIM_DNS_NODE ]; then
	echo "Please define DIM_DNS_NODE"
	exit 1
fi
if [ $# -ge 1 ]; then
	if [ $1 = "dbg" ]; then
		DBG=1
	fi
fi
mkdbgscr () {
cat > ${ONLINETASKSROOT}/job/meprxgdb.x <<EOF
file ${GAUDIONLINEROOT}/${CMTCONFIG}/Gaudi.exe
b 'LHCb::MEPRxSvc::initialize()'
run libGaudiOnline.so OnlineTask -msgsvc=MessageSvc -auto -opt=$ONLINETASKSROOT/options/MEPRxSvc.opts -main=$GAUDIONLINEROOT/options/Main.opts
EOF
#	file ${GAUDIONLNEROOT}/${CMTCONFIG}/Gaudi.exe
#	b LHCb::MEPRxSvc::initialize
#	run libGaudiOnline.so OnlineTask -msgsvc=MessageSvc -auto -opt=$ONLINETASKSROOT/options/MEPRxSvc.opts -main=$GAUDIONLINEROOT/options/Main.opts
 
}   

# Cleanup any running jobs
pkill Gaudi.exe
pkill test.exe
pkill gentest.exe
pkill logViewer
pkill xterm
HOST=/$(hostname --short | awk '{ print toupper($1) }')
# message are send via the DIM Messageservice
#export MSGSVC=LHCb::MessageSvc
export MSGSVC=LHCb::FmcMessageSvc

# TAN - do not touch unless you know what you're doing
export TAN_PORT YES
export TAN_NODE=$(hostname -f)

export BIGTERM='xterm  -hold -ls -132 -geometry 132x45 -title ' 
export WIDETERM='xterm -hold -vb -j -sl 10000 -ls -132 -geometry 160x40-0-0 -title '
export MINITERM='xterm -iconic -sl 10000 -ls -132 -geometry 132x10 -title '

# shortcuts for starting jobs
export gaudi_run="${GAUDIONLINEROOT}/${CMTCONFIG}/Gaudi.exe libGaudiOnline.so OnlineStart "
export gaudi_exe="${GAUDIONLINEROOT}/${CMTCONFIG}/Gaudi.exe libGaudiOnline.so OnlineTask -msgsvc=${MSGSVC} -auto"  
export gaudi_exe2="${GAUDIONLINEROOT}/${CMTCONFIG}/Gaudi.exe libGaudiOnline.so OnlineTask -msgsvc=MessageSvc -auto"  
 
# Online tasks - be careful when changing the startup order 
#
# ErrorLogger (not required for running)
#$WIDETERM ErrorLogger@${HOST}    -e "export UTGID=${HOST}/ErrLog; /opt/FMC/bin/logViewer -N $(hostname) " &
# 
# MBMInit initializes the shared memory
$BIGTERM MEPInit@${HOST} -e "export UTGID=${HOST}/MEPInit ; ${gaudi_exe2} -main=${ONLINETASKSROOT}/options/MEPInitNew.opts  -opt=$GAUDIONLINEROOT/options/Daemon.opts " &
#
#  Monitors:
#
$BIGTERM MBMMon@${HOST} -e "export UTGID=${HOST}/MBMMon ; ${gaudi_run} libOnlineKernel.so mbm_mon" &
#
sleep 2
#
#
# At least one Consumer is needed - for example the (dummy) Moore process
#
#$WIDETERM Moore_0@${HOST}   -e "export UTGID=${HOST}/Moore_0  ; ${gaudi_exe2} -opt=$ONLINETASKSROOT/options/ReadMBM.opts -main=$GAUDIONLINEROOT/options/Main.opts "&
#$MINITERM Moore_1@${HOST}   -e "export UTGID=${HOST}/Moore_1  ; ${gaudi_exe} -opt=$GAUDIONLINEROOT/options/ReadMBM.opts -main=$GAUDIONLINEROOT/options/Main.opts "&
#$MINITERM Moore_2@${HOST}   -e "export UTGID=${HOST}/Moore_2  ; $gaudi_exe -opt=$GAUDIONLINEROOT/options/ReadMBM.opts -main=$GAUDIONLINEROOT/options/Main.opts "&
# $MINITERM Moore_3@${HOST}   -e "export UTGID=${HOST}/Moore_3  ; $gaudi_exe -opt=$GAUDIONLINEROOT/options/ReadMBM.opts -main=$GAUDIONLINEROOT/options/Main.opts "&
#
# Optional if you want write the data
#
sleep 1
#$BIGTERM DiskWR@${HOST}    -e "export UTGID=${HOST}/DiskWR   ; ${gaudi_exe} -opt=$ONLINETASKSROOT/options/DiskWRLite.opts -main=$GAUDIONLINEROOT/options/Main.opts "&
#
# Last not least the event-builder
#
#pid=$(ps waux | awk "/[0-9]+ ${WIDETERM}.*meprx/ { print \$2 }")
#[ -z $pid ] && $WIDETERM MEPRx@${HOST} -e "/opt/FMC/bin/logViewer | grep -i meprx" &
#$WIDETERM MEPRx@${HOST} -e "export UTGID=${HOST}/MEPRx ; ${gaudi_exe} -opt=$ONLINETASKSROOT/options/MEPRxSvc.opts -main=$GAUDIONLINEROOT/options/Main.opts "&
sleep 1
export UTGID=MEPRX
if [[ ! -z $DBG ]]; then
	mkdbgscr
	gdb -x ${ONLINETASKSROOT}/job/meprxgdb.x
else
	${gaudi_exe2} -opt=$ONLINETASKSROOT/options/MEPRxSvcNew.opts -main=$GAUDIONLINEROOT/options/Main.opts
fi 

