#!/bin/bash
. /group/online/dataflow/scripts/preamble.sh
. ${DATAFLOWDIR}/scripts/cleanEnviron.sh ROOTSYS DIM_DNS_NODE
rm -f /dev/shm/RONodes
exec -a ${UTGID} `which gentest.exe` libROMon.so romon_node_collect -gbl=RONodes -size=1024 -delay=500 -match=ROPublish_*
