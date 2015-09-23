#!/bin/bash
#echo "CMTCONFIG=$CMTCONFIG "
#. setup.sh
export LD_PRELOAD=libCheckpointing.so
#echo ${LD_LIBRARY_PATH}
#echo ${PATH}
#echo ${LD_PRELOAD}
`which gentest.exe` libCheckpointing.so $*
