#!/bin/bash
. /group/online/dataflow/scripts/nodeMonitors.sh
. ${DATAFLOWDIR}/scripts/cleanEnviron.sh ROOTSYS DIM_DNS_NODE
/bin/sleep 10
#echo "UTGID is: ${UTGID}"
SVC=/`hostname -s`/${UTGID}
#exec -a ${UTGID} `which gentest.exe` libROMon.so romon_dimserver -gbl=ROMonitor -size=64 -delay=500 -maponly -publish=${SVC}
