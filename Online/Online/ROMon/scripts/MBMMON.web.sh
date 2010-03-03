#!/bin/bash
. /group/online/dataflow/scripts/preamble.sh
. ${DATAFLOWDIR}/scripts/cleanEnviron.sh HOST DIM_DNS_NODE LD_LIBRARY_PATH
exec -a ${UTGID} `which test.exe` mbm_mon_serv
