#!/bin/bash
. /group/online/dataflow/scripts/nodeMonitors.sh
. ${DATAFLOWDIR}/scripts/cleanEnviron.sh ROOTSYS DIM_DNS_NODE ROMONROOT
#echo $LD_LIBRARY_PATH
#echo $ROMONROOT
cd ${ROMONROOT}
h=`hostname -s`
H=`echo $h|tr a-z A-Z`
exec -a ${UTGID} `which gentest.exe` libROMon.so run_tasksupervisor \
    -input=file://${ROMONROOT}/xml/${H}.xml \
    -invent=file://${ROMONROOT}/xml/TaskInventory.xml \
    -node=${h} $*
