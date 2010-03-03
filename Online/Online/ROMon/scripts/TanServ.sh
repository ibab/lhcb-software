#!/bin/bash
if test -z "$UTGID";
then
  UTGID=P$$;
fi;
export UTGID
. /group/online/dataflow/scripts/preamble.sh
. ${DATAFLOWDIR}/scripts/cleanEnviron.sh DATAINTERFACE
export TAN_PORT=YES
export TAN_NODE=${DATAINTERFACE}
#
echo "TAN: Host: $HOST  $TAN_NODE $TAN_PORT Data interface:$DATAINTERFACE"
exec -a ${UTGID} `which test.exe` tan_nameserver -a -tcp -d
