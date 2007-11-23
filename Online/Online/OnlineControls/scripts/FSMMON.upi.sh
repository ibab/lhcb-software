#!/bin/bash
export ONLINEDIR=`dirname $0`/../..
cd $ONLINEDIR/OnlineControls/cmt
. setup.vars
export PVSS_II=/localdisk/pvss/${1}/config/config
export LD_LIBRARY_PATH=$ONLINEDIR/UPIR/${CMTCONFIG}:$LD_LIBRARY_PATH
export EXECS=${PATH}
echo $PYTHONPATH
the_items=`printenv \
| grep -v HOST \
| grep -v ONLINEDIR \
| grep -v EXECS \
| grep -v PYTHONPATH \
| grep -v PYTHONHOME \
| grep -v ROOTSYS \
| grep -v LD_LIBRARY_PATH \
| grep -v DIM_DNS_NODE \
| grep -v PVSS`
export PYTHONPATH=`python -c "import os;print os.environ['PYTHONPATH'].replace(' :',':').replace(': ',':')"`
eval `python -c "for i in \"\"\"$the_items\"\"\".split('\n'): print 'unset '+i.split('=')[0]"`
export PATH=$EXECS
export UTGID=FSMMON_0
export TAN_PORT=YES
export DATAINTERFACE=`python $ONLINEDIR/OnlineControls/scripts/getDataInterface.py`
export TAN_NODE=${DATAINTERFACE}
#
unset ONLINEDIR
unset EXECS
export LD_PRELOAD=${PYTHONHOME}/lib/libpython2.4.so
exec -a ${UTGID} ${PVSS00api} \
-NAME FSMMON_0 \
-DLL PVSSInterface \
-FUN pvss_pymain \
-num 52 \
-proj ${1} \
-SCRIPT ../python/Online/Streaming/UPIDisplay.py
