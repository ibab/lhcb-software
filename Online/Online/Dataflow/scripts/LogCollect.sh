#!/bin/bash
export UTGID
export PATH=/opt/FMC/sbin:/opt/FMC/bin:${PATH};
export LD_LIBRARY_PATH=/opt/FMC/lib;
. /group/online/dataflow/scripts/cleanEnviron.sh DIM_DNS_NODE
exec -a ${UTGID} /opt/FMC/bin/logViewer -l 3 -o /tmp/logsum.fifo
