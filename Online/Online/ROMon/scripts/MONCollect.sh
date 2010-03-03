#!/bin/bash
. /group/online/dataflow/scripts/nodeMonitors.sh
. ${DATAFLOWDIR}/scripts/cleanEnviron.sh ROOTSYS DIM_DNS_NODE
exec -a ${UTGID} `which gentest.exe` libROMon.so $*
