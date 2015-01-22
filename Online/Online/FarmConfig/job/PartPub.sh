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
#export PYTHONPATH=${ONLINE_ENV_DIR}:${PYTHONPATH}
#export AdderOptsFile=/tmp/${PARTITION_NAME}_PublisherOpts.opts
#python ./Tasklist_from_architecture.py 4 ${AdderOptsFile}
exec -a ${UTGID} ${Class1_task} -opts=../options/pooper.opts
