#!/bin/bash
export UTGID
. /group/online/dataflow/scripts/nodeMonitors.sh
## . /home/frankm/cmtuser/Online_v4r11/Online/ROLogger/cmt/setup.slc4_ia32_gcc34_dbg.vars
if test -d /localdisk/pvss/${1}/log; then
  exec -a ${UTGID} gentest.exe libROLogger.so romon_pvss_logger /localdisk/pvss/${1}/log;
else
  exec -a ${UTGID}_sleeper sleep 200000;
fi;

