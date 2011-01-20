#!/bin/bash
#echo "CMTCONFIG=$CMTCONFIG "
#. setup.sh
export LD_PRELOAD=${CHECKPOINTINGROOT}/${CMTCONFIG}/libCheckpointing.so
#echo ${LD_LIBRARY_PATH}
#echo ${PATH}
#echo ${LD_PRELOAD}
`which gentest.exe` libCheckpointing.so chkpt_tests -p 3 -n -f t_checkpoint
