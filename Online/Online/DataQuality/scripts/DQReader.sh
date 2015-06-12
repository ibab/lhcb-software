#!/bin/bash
#
export NODE_OPTIONS=/group/online/dataflow/cmtuser/DQ/DATAQUALITY_OPT/ReaderInput.opts;
exec -a ${UTGID} ${Class2_task} -opts=../options/${TASK_TYPE}.opts
