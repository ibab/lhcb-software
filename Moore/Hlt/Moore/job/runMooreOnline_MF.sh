#!/bin/sh

export PARENT=$1
# remove the args because they interfere with the cmt scripts
export HOME=/home/$(/usr/bin/whoami)
# pick up 'our' setup... 
source /group/hlt/MOORE/Moore_v13r4/Hlt/Moore/job/SetupMoore_v13r4.sh

export DIM_DNS_NODE=mona08
echo ${UTGID} Running as $(/usr/bin/whoami) with DIM_DNS_NODE $DIM_DNS_NODE and home $HOME , cmtconfig $CMTCONFIG

# export LOGFIFO=/tmp/logGaudi.fifo

# define the task
export gaudi_exe="$GAUDIONLINEROOT/$CMTCONFIG/Gaudi.exe $GAUDIONLINEROOT/$CMTCONFIG/libGaudiOnline.so OnlineTask -msgsvc=LHCb::FmcMessageSvc"  
export CLASS1_TASK="${gaudi_exe} -tasktype=LHCb::Class1Task -main=/group/online/dataflow/templates/options/Main.opts"

# pick up partition specific OnlineEnv module
export PYTHONPATH=/group/online/dataflow/options/${PARTITION}/MONITORING:${PYTHONPATH}
# and go for it!
exec -a ${UTGID} ${CLASS1_TASK} -opt=command="import Moore.runOnline_MF; Moore.runOnline_MF.start()"

