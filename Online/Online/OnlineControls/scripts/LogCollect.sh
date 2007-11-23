#!/bin/bash
EXECS=/opt/FMC/sbin:/opt/FMC/bin:${PATH};
export LD_LIBRARY_PATH=/opt/FMC/lib;
the_items=`printenv | grep -v UTGID | grep -v LD_LIBRARY_PATH | grep -v DIM_DNS_NODE`
eval `python -c "for i in \"\"\"$the_items\"\"\".split('\n'): print 'unset '+i.split('=')[0]"`
export PATH=${EXECS}
exec -a ${UTGID} /opt/FMC/bin/logViewer -l 3 -o /tmp/logsum.fifo
