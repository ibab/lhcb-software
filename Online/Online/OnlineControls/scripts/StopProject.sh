#!/bin/bash
cd /home/frankm/cmtuser/Gaudi_v19r2/Online/OnlineControls/cmt
source PVSS.setup.sh
unset PVSS_II
echo "Stop project: `date`"
export PVSS_II=/localdisk/pvss/REC${1}/config/config
${PVSS_SYSTEM_ROOT}/bin/PVSS00pmon -proj REC${1} -stopWait
echo "Project stopped: REC${1} `date`"
