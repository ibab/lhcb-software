#!/bin/sh

export PARENT=$1
# remove the args because they interfere with the cmt scripts
export HOME=/home/$(/usr/bin/whoami)
# Setup the environment.
export User_release_area=/group/hrc/software/cmtuser
source $User_release_area/HERSCHEL/HERSCHEL_v1r0/HerschelSys/cmt/setup.x86_64-slc6-gcc48-opt.vars
#source $User_release_area/HERSCHEL/HERSCHEL_v1r0/HerschelSys/cmt/setup.x86_64-slc6-gcc48-dbg.vars

export DIM_DNS_NODE=mona08
echo ${UTGID} Running as $(/usr/bin/whoami) with DIM_DNS_NODE $DIM_DNS_NODE and home $HOME, cmtconfig $CMTCONFIG

# export LOGFIFO=/tmp/logGaudi.fifo

# Define the task
ONLINE_INSTALL=${GAUDIONLINEROOT}/../../InstallArea/${CMTCONFIG}
export gaudi_exe="$ONLINE_INSTALL/bin/GaudiOnlineExe.exe $ONLINE_INSTALL/lib/libGaudiOnline.so OnlineTask -msgsvc=LHCb::FmcMessageSvc"
export CLASS1_TASK="${gaudi_exe} -tasktype=LHCb::Class1Task -main=/group/online/dataflow/templates/options/Main.opts"

# pick up partition specific OnlineEnv module
export PYTHONPATH=/group/online/dataflow/options/${PARTITION}/MONITORING:${PYTHONPATH}
# and go for it!
exec -a ${UTGID} ${CLASS1_TASK} -opt=command="import HCMonitors.runOnlineMonitor; HCMonitors.runOnlineMonitor.start(True)"
#gdb --args ${CLASS1_TASK} -opt=command="import HCMonitors.runOnlineMonitor; HCMonitors.runOnlineMonitor.start(True)"
