#!/bin/bash
/home/frankm/cmtuser/Online_v5r14/InstallArea/x86_64-slc6-gcc48-dbg/bin/gentest.exe libController.so run_monitor_test \
-print=4 \
-scan=100 \
-instances=10 \
-dim -avg=30000 \
-min_file_evt=10 \
-events=10000 \
-min_data_evt=1000 \
-name=HLT2Monitoring \
-task=FileRawSizeMon \
-reconnect
