#!/bin/bash
#cd /home/frankm/raul/Online_v4r12/MyTest/cmt
cd `dirname $0`
if test "`uname -i`" = "i386";
    then
    . setup.slc4_ia32_gcc34_dbg.vars
    export PYTHONHOME=/sw/lib/lcg/external/Python/2.5/slc4_ia32_gcc34
else
    . setup.slc4_amd64_gcc34_dbg.vars
    export PYTHONHOME=/sw/lib/lcg/external/Python/2.5/slc4_amd64_gcc34
fi;
export UTGID
export PYTHONPATH=$PYTHONPATH:$LD_LIBRARY_PATH
`which python` ../python/TaskSupervisor.py
#exec -a ${UTGID} `which python` ../python/TaskSupervisor.py
