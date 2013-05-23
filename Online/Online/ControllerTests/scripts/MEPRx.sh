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
export PYTHONPATH=${HOME}/data:${PYTHONPATH};
exec -a ${UTGID} ${Class2_task} -opts=../options/OverflowReader.opts
