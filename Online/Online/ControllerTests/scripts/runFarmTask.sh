#!/bin/bash
# =========================================================================
#
#  Default script to bootstrap any task running in the HLT farm
#
#  Arguments:
#  1:   Task type
#  2:   Partition name
#  3:   --
#
#  Author   M.Frank
#  Version: 1.0
#  Date:    20/05/2013
#
# =========================================================================
#
. ../cmt/setup.x86_64-slc5-gcc46-dbg.vars
. ./preamble.sh
export UTGID=${UTGID}
export STATIC_OPTS=../options
export DYNAMIC_OPTS=../options
#
eval `python <<EOF
args = "$*".split()
while len(args):
  type = args[0]
  del args[0]
  if type == '-type': 
    print 'export TASK_TYPE='+args[0]+';'
  elif type == '-partition':
    print 'export PARTITION='+args[0]+';'
  elif type == '-runinfo':
    print 'export RUNINFO='+args[0]+';'
  elif type == '-instances':
    print 'export NBOFSLAVES='+args[0]+';'
  else:
    print 'echo Unknown task argument '+type+' '+args[0]+';'
  del args[0]
EOF`
##echo $UTGID -- $* Invoking script: ${2}.sh
#
export DATAINTERFACE=`python /group/online/dataflow/scripts/getDataInterface.py`
export ONLINETASKS=/group/online/dataflow/templates;
export INFO_OPTIONS=${DYNAMIC_OPTS}/OnlineEnvironment.opts;
export MBM_SETUP_OPTIONS=${DYNAMIC_OPTS}/MBM_setup.opts;
export SUBFARM_OPTIONS=${DYNAMIC_OPTS}/PLUS04.opts;
#
export LOGFIFO=/tmp/logSrv.fifo;
#
. ./${TASK_TYPE}.sh
