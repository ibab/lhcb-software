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
export AdderOptsFile=/tmp/${PARTITION_NAME}_AdderOpts.opts
python ./Tasklist_from_architecture.py ${ARCH_FILE} 2 ${AdderOptsFile}
exec -a ${UTGID} ${Class1_task} -opts=../options/genAdder.opts
