#!/bin/bash
echo $1
echo $2
export HOME=/home/online
export USER=online # these dumb offline scripts....
source /cvmfs/lhcb.cern.ch/lib/LbLogin.sh
source /group/online/dataflow/cmtuser/AlignmentOnlineDev_v10r3/setup.x86_64-slc6-gcc48-dbg.vars
export PATH=/group/online/dataflow/cmtuser/LHCbDev_v38r5/InstallArea/x86_64-slc6-gcc48-dbg/bin:/group/online/dataflow/cmtuser/LHCbDev_v38r5/InstallArea/x86_64-slc6-gcc48-dbg/scripts:$PATH
export PYTHONPATH=$1/..:$1:/group/online/hlt/conditions/RunChangeHandler:/group/online/dataflow/cmtuser/LHCbDev_v38r5/InstallArea/x86_64-slc6-gcc48-dbg/python:/group/online/dataflow/cmtuser/LHCbDev_v38r5/InstallArea/x86_64-slc6-gcc48-dbg/python/lib-dynload:/group/online/dataflow/cmtuser/LHCbDev_v38r5/InstallArea/x86_64-slc6-gcc48-dbg/python.zip:$PYTHONPATH
unset GAUDIAPPNAME
source /sw/oracle/set_oraenv.sh
export CORAL_DBLOOKUP_PATH=/group/online/conddbserver/
export CORAL_AUTH_PATH=/group/online/conddbserver
python $ALIGNONLINEROOT/python/DBXfer.py $2
exit $?
