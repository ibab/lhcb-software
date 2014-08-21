#!/bin/bash
# =========================================================================
#
#  Default script to start the event reader task on the HLT farm
#
#  Author   M.Frank
#  Version: 1.0
#  Date:    20/05/2013
#
# =========================================================================
#
export PYTHONPATH=${ONLINE_ENV_DIR}:${PYTHONPATH}
export AdderOptsFile=/tmp/${PARTITION_NAME}_AdderOpts.opts
python ./Tasklist_from_architecture.py 1 ${AdderOptsFile}
export LD_LIBRARY_PATH=/home/beat/cmtuser/myOnline/InstallArea/x86_64-slc6-gcc48-dbg/lib/:$LD_LIBRARY_PATH
exec -a ${UTGID} ${Class1_task} -opts=../options/genAdder.opts
