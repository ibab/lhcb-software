#!/bin/bash
echo $1
echo $2
echo $3
source /group/online/dataflow/cmtuser/ALIGNMENTONLINE/ALIGNMENTONLINE_HEAD/AlignmentOnline/AlignOnline/cmt/setup.x86_64-slc6-gcc48-dbg.vars
export PYTHONPATH=$1:/group/online/hlt/conditions/RunChangeHandler:$PYTHONPATH
unset GAUDIAPPNAME
python $ALIGNONLINEROOT/python/DBXfer.py
#exit 0
