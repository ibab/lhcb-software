#!/bin/bash
EXECS=/opt/FMC/sbin:/opt/FMC/bin:${PATH};
export LD_LIBRARY_PATH=/opt/FMC/lib;
the_items=`printenv | grep -v UTGID | grep -v LD_LIBRARY_PATH`
eval `python -c "for i in \"\"\"$the_items\"\"\".split('\n'): print 'unset '+i.split('=')[0]"`
export DIM_DNS_NODE=${1}
#echo "SERVER DNS NODE: ${DIM_DNS_NODE} "
#echo "${UTGID} DIM Dns node is: ${DIM_DNS_NODE} "
exec -a ${UTGID} /opt/FMC/sbin/logSrv -l 3 -p /tmp/logsum.fifo
