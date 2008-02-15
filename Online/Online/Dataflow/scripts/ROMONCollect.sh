#!/bin/bash
. /group/online/dataflow/scripts/preamble.sh
. ${DATAFLOWDIR}/scripts/cleanEnviron.sh ROOTSYS DIM_DNS_NODE
rm -f /dev/shm/ROMonitor
exec -a ${UTGID} `which gentest.exe` libROMon.so romon_collect -gbl=ROMonitor -size=64 -delay=500 -verbose
