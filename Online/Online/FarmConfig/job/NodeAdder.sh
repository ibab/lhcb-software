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
exec -a ${UTGID} ${Class1_task} -opts=../options/NodeAdder.opts
