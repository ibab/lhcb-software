#!/bin/bash
. /group/online/dataflow/scripts/nodeMonitors.sh
. ${DATAFLOWDIR}/scripts/cleanEnviron.sh ROOTSYS DIM_DNS_NODE ROMONDATA
h=`hostname -s`
H=`echo $h|tr a-z A-Z`
exec -a ${UTGID} `which gentest.exe` libROMon.so run_tasksupervisor \
    -input=file://${ROMONDATA}/${H}.xml \
    -invent=file://${ROMONDATA}/TaskInventory.xml \
    -node=${h} $*
