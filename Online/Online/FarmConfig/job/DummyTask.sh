#!/bin/bash
# =========================================================================
#
#  Default script to start a dummy task only following FSM transitions
#
#  Author   M.Frank
#  Version: 1.0
#  Date:    20/05/2013
#
# =========================================================================
#
export LOGFIFO=/tmp/logSrv.fifo;
exec -a ${UTGID} ${Class1_task} -opt=../options/DummyTask.opts
