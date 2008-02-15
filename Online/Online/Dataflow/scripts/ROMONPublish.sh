#!/bin/bash
. /group/online/dataflow/scripts/preamble.sh
. ${DATAFLOWDIR}/scripts/cleanEnviron.sh ROOTSYS DIM_DNS_NODE
/bin/sleep 3
exec -a ${UTGID} `which gentest.exe` libROMon.so romon_dimserver -gbl=ROMonitor -size=64 -delay=500 -maponly -publish=${UTGID}
