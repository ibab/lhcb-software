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
export IP_DATAINTERFACE=`python -c "import socket;print socket.gethostbyname(socket.gethostname().split('.')[0]+'-d1')"`
export TELL1BOARDS=${DYNAMIC_OPTS}/${PARTITION_NAME}_Tell1Boards.opts;
exec -a ${UTGID} ${Class2_task} -opts=../options/${TASK_TYPE}.opts
