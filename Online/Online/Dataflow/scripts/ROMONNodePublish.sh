#!/bin/bash
. /group/online/dataflow/scripts/preamble.sh
export DIM_DNS_NODE=${1}
. ${DATAFLOWDIR}/scripts/cleanEnviron.sh ROOTSYS DIM_DNS_NODE
/bin/sleep 3
exec -a ${UTGID} `which gentest.exe` libROMon.so romon_dimserver -gbl=RONodes -size=1024 -delay=1500 -publish=${UTGID} -maponly
