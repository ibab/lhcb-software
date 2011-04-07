#!/bin/bash
. /group/online/dataflow/scripts/nodeMonitors.sh
### . ${ROMONROOT}/scripts/PingSrv.py
. ${DATAFLOWDIR}/scripts/cleanEnviron.sh ROOTSYS DIM_DNS_NODE ROMONDATA
##h=`hostname -s`
h=${DIM_DNS_NODE};
H=`echo $h|tr a-z A-Z`;
if test -f ${ROMONDATA}/${H}.xml; then
  exec -a ${UTGID} `which gentest.exe` libROMon.so run_node_ping \
      -input=file://${ROMONDATA}/${H}.xml \
      -invent=file://${ROMONDATA}/TaskInventory.xml $*;
else
  exec -a ${UTGID}_noXML_sleeper sleep 200000;
fi;
