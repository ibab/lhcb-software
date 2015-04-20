#!/bin/bash
#
. /home/frankm/cmtuser/OnlineDev_v5r17/setup.x86_64-slc6-gcc48-dbg.vars;
export PY_EXE=`which python`;
export PYTHONHOME=`dirname $PY_EXE`;
export PYTHONHOME=`dirname $PYTHONHOME`;
#
exec -a ${UTGID} `which python` -c "import HLT2.DimInterface as FSM; FSM.runVoidTask(name='${UTGID}',auto=True)";
