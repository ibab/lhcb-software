#!/bin/bash
ONLINEDIR=`dirname $0`/../..
cd $ONLINEDIR/UPIR/cmt
. setup.vars
cd ../${CMTCONFIG}
if test ! -e libUPI.so;
then
  echo "You have to Link libUPIR to libUPI !!!!!!!!!!!!!!!! (Needs to be done by hand)";
fi;
export LD_LIBRARY_PATH=$ONLINEDIR/UPIR/${CMTCONFIG}:$LD_LIBRARY_PATH
export EXECS=${PATH}
the_items=`printenv \
| grep -v HOST \
| grep -v ONLINEDIR \
| grep -v LD_LIBRARY_PATH \
| grep -v EXECS`
eval `python -c "for i in \"\"\"$the_items\"\"\".split('\n'): print 'unset '+i.split('=')[0]"`
export PATH=$EXECS
export UTGID=MBMMON_0
export DATAINTERFACE=`python $ONLINEDIR/OnlineControls/scripts/getDataInterface.py`
export TAN_PORT=YES
export TAN_NODE=${DATAINTERFACE}
#
unset ONLINEDIR
unset EXECS
exec -a MBMMON_0 `which gentest.exe` libUPI.so run_mbm_upi
