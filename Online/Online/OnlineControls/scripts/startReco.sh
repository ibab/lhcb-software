#!/bin/bash
cd `dirname $0`/../cmt
. PVSS.setup
export PROJECT=REC`hostname -s|tr [a-z] [A-Z]`
export PVSS_II=/localdisk/pvss/${PROJECT}/config/config
chmod -R g+rwx /localdisk/pvss/${PROJECT}/
chmod -R o+rwx /localdisk/pvss/${PROJECT}/
chmod -R a+rwx /localdisk/pvss/${PROJECT}/
${PVSS_SYSTEM_ROOT}/bin/PVSS00pmon -config /localdisk/pvss/${PROJECT}/config/config -autoreg&
