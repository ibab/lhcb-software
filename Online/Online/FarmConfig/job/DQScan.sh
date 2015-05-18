#!/bin/bash
#
cd /home/frankm/cmtuser/OnlineDev_v5r17;
. ./setup.x86_64-slc6-gcc48-dbg.vars
export PY_EXE=`which python`;
export PYTHONHOME=`dirname $PY_EXE`;
export PYTHONHOME=`dirname $PYTHONHOME`;
export UTGID;
export DIM_DNS_NODE=mona10;
export PYTHONPATH=/home/frankm/cmtuser/OnlineDev_v5r17/Online/Controller/python:$PYTHONPATH;
#
echo "DIM-DNS: $DIM_DNS_NODE  Working directory: `pwd`"
#
cd /tmp;
exec -a ${UTGID} `which python` -c "import HLT2.OfflineMonitoringScanner as Scan; Scan.run()";
