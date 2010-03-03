#!/bin/bash
export UTGID
. /group/online/dataflow/scripts/nodeMonitors.sh
## . /home/frankm/cmtuser/Online_v4r11/Online/ROLogger/cmt/setup.slc4_ia32_gcc34_dbg.vars
exec -a ${UTGID} gentest.exe libROLogger.so romon_pvss_logger /localdisk/pvss/${1}/log 
