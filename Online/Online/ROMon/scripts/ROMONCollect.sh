#!/bin/bash
. /group/online/dataflow/scripts/nodeMonitors.sh
. ${DATAFLOWDIR}/scripts/cleanEnviron.sh ROOTSYS DIM_DNS_NODE
#####set -x
rm -f /dev/shm/ROMonitor
#exec -a ${UTGID} `which gentest.exe` libROMon.so romon_collect -gbl=ROMonitor -size=64 -delay=500 -verbose;
SVC=/`hostname -s`/${UTGID};
exec -a ${UTGID} `which gentest.exe` libROMon.so romon_syscollect -size=64 -statDelay=8000 -mbmDelay=500 -pub=${SVC};
