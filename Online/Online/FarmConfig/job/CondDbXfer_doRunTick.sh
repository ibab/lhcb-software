#!/bin/bash
echo $1
echo $2
export HOME=/home/online
export USER=online # these dumb offline scripts....
source /cvmfs/lhcb.cern.ch/lib/LbLogin.sh
source /group/online/dataflow/cmtuser/AlignmentOnlineDev_v10r5/setup.x86_64-slc6-gcc48-dbg.vars
export PYTHONPATH=$1/..:$1:/group/online/hlt/conditions/RunChangeHandler:$PYTHONPATH
unset GAUDIAPPNAME
source /sw/oracle/set_oraenv.sh
export CORAL_DBLOOKUP_PATH=/group/online/conddbserver/
export CORAL_AUTH_PATH=/group/online/conddbserver
echo $2
python $ALIGNONLINEROOT/python/DBXfer_doRunTick.py $2
exit $?
