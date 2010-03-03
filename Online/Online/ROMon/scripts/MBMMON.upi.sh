#!/bin/bash
. /group/online/dataflow/scripts/preamble.sh
. ${DATAFLOWDIR}/scripts/cleanEnviron.sh DATAINTERFACE
export TAN_PORT=YES
export TAN_NODE=${DATAINTERFACE}
exec -a ${UTGID} `which gentest.exe` libUPIR.so run_mbm_upi
