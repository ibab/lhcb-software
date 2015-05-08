#!/bin/bash
CURR=`dirname $0`;
cd $CURR/../python;
CURR=`pwd`;
export PYTHONPATH=/home/frankm/cmtuser/Online_v5r14/InstallArea/x86_64-slc6-gcc48-opt/python:$CURR:$PYTHONPATH
python -c "import HLT2.HLT2web as w;w.run_web()"
