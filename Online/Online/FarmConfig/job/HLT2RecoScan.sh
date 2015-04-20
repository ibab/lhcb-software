#!/bin/bash
#
. /home/frankm/cmtuser/OnlineDev_v5r17/setup.x86_64-slc6-gcc48-dbg.vars
export PY_EXE=`which python`;
export PYTHONHOME=`dirname $PY_EXE`;
export PYTHONHOME=`dirname $PYTHONHOME`;
#
cd /home/frankm/cmtuser/OnlineDev_v5r17;
cd /tmp;
echo "DIM-DNS: $DIM_DNS_NODE"
#
exec -a ${UTGID} `which python` -c "import HLT2.OfflineMonitoringScanner as Scan; Scan.run()";
