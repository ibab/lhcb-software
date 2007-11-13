#!/bin/bash
export ONLINEDIR=/group/online/dataflow/cmtuser/Gaudi_v19r2/Online
cd $ONLINEDIR/UPI/cmt
. setup.vars
EXECS=$PATH
the_items=`printenv \
| grep -v HOST \
| grep -v LD_LIBRARY_PATH'
eval `python -c "for i in \"\"\"$the_items\"\"\".split('\n'): print 'unset '+i.split('=')[0]"`
export PATH=$EXECS
export TAN_PORT=YES
export TAN_NODE=${HOST}
export UTGID=UpiServ_$$
xterm  -ls -132 -geometry 132x45 -title $UTGID -e "export UTGID=${UTGID} `which gentest.exe` libUPI.so upi_server" &
