#!/bin/bash
export UTGID
export DIM_DNS_NODE=${1}
export PATH=/opt/FMC/sbin:/opt/FMC/bin:${PATH};
export LD_LIBRARY_PATH=/opt/FMC/lib;
. /group/online/dataflow/scripts/cleanEnviron.sh DIM_DNS_NODE
exec -a ${UTGID} /opt/FMC/sbin/logSrv -l 3 -p /tmp/logsum.fifo
