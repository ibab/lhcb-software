#!/bin/bash
. /group/online/dataflow/scripts/nodeMonitors.sh
export DIM_DNS_NODE=${1}
shift;
. ${DATAFLOWDIR}/scripts/cleanEnviron.sh ROOTSYS DIM_DNS_NODE
/bin/sleep 10
# echo `which gentest.exe` libROMon.so romon_dimserver -publish=${UTGID} $*
h=`hostname -s`
SVC=/${h}/${UTGID}
if test "`echo ${h}|cut -b -5`" = "store";
then
 h="store";
fi;
echo "Match is: /${h}*/ROcollect"
exec -a ${UTGID} `which gentest.exe` libROMon.so romon_syspublish -match=/${h}*/ROcollect -publish=${SVC} $*
