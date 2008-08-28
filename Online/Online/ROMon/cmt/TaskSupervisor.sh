#!/bin/bash
cd /home/frankm/raul/Online_v4r12/MyTest/cmt
. setup.slc4_amd64_gcc34.vars
export UTGID
export PYTHONHOME=/sw/lib/lcg/external/Python/2.5/${CMTCONFIG}
exec -a $UTGID `which python` ../src/TaskSupervisor.py
