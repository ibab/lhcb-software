#!/bin/bash
export ONLINEDIR=`dirname $0`/../..
cd $ONLINEDIR/OnlineKernel/cmt
. setup.vars
EXECS=$PATH
the_items=`printenv \
| grep -v HOST \
| grep -v ONLINEDIR \
| grep -v LD_LIBRARY_PATH \
| grep -v DIM_DNS_NODE`
eval `python -c "for i in \"\"\"$the_items\"\"\".split('\n'): print 'unset '+i.split('=')[0]"`
export PATH=$EXECS
export UTGID=TANServ_0
export DATAINTERFACE=`python $ONLINEDIR/OnlineControls/scripts/getDataInterface.py`
export TAN_PORT=YES
export TAN_NODE=${DATAINTERFACE}
echo "PAH= $PATH test=`which test.exe`"

echo "TAN: Host: $HOST  Data interface:$DATAINTERFACE"
exec -a $UTGID `which test.exe` tan_nameserver -a -tcp -d
