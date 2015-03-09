#!/bin/bash
cd `dirname $0`/../cmt;
. setup.x86_64-slc6-gcc48-dbg.vars;
#
cd ~/HLT2mon;
##/home/frankm/bin/debug2 --args \
`which gentest.exe` libController.so run_monitor_test \
-print=4 \
-scan=100 \
-instances=10 \
-dim -avg=30000 \
-min_file_evt=10 \
-events=10000 \
-min_data_evt=1000 \
-base=/home/frankm/HLT2mon \
-name=HLT2Monitoring \
-task=FileRawSizeMon \
-reconnect
