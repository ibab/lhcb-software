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
#source /group/online/dataflow/cmtuser/OnlineDev_v5r17/setup.x86_64-slc6-gcc48-dbg.vars
export victimnodes="hlt02(.*)"
export PYTHONPATH=${ONLINE_ENV_DIR}:${PYTHONPATH}
export AdderOptsFile=/tmp/${PARTITION_NAME}_AdderOpts.opts
export AdderStaticOpts=../options/PartAdderStatic.opts
python ./Tasklist_from_architecture.py 3 ${AdderOptsFile}
exec -a ${UTGID} ${Class1_task} -opts=../options/genAdder.opts
