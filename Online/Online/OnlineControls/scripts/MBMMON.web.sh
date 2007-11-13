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
export UTGID=MBMWeb_0
exec -a $UTGID `which test.exe` mbm_mon_serv
