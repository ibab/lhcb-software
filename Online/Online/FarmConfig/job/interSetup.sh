#!/bin/bash
# =========================================================================
#
#  Default script to start any task on the HLT farm.
#
#  Author   M.Frank
#  Version: 1.0
#  Date:    20/05/2013
#
# =========================================================================
#

#from /group/online/dataflow/runFarmTask.sh
echo "===========>from /group/online/dataflow/runFarmTask.sh"

#!/bin/bash
# =========================================================================
#
#  Generic farm task startup script
#
#  Author   M.Frank
#  Version: 1.0
#  Date:    20/05/2013
#
# =========================================================================
export UTGID=${UTGID}
#
#  Evaluate the information in the runinfo file
#  --------------------------------------------
eval `python <<EOF
import os, sys, imp

dbg = False

def _p(x):
  if dbg: print 'echo '+x+';'
  print x+';'

args = "$*".split()
print args
runinfo = None
if len(args) == 1:
  partition_name = args[0]
  print partition_name
if runinfo is None:
  runinfo = '/group/online/dataflow/options/'+partition_name+'/HLT/OnlineEnvBase.py'
  _p('export RUNINFO='+runinfo)
try:
  #_p('echo '+runinfo)
  dir,fname = os.path.split(runinfo)
  nam,ext = os.path.splitext(fname)
  sys.path.insert(1,dir)
  mod = imp.find_module(nam,None)
  Online=imp.load_module(nam,mod[0],mod[1],mod[2])
  archfile = '/group/online/dataflow/architectures/lbDataflowArch_'+str(Online.HltArchitecture)+'.xml'
  _p('export ARCH_FILE='+archfile)
  _p('export LOGFIFO='+str('/tmp/logGaudi.fifo'))
  _p('export ONLINE_ENV_DIR='+dir)
  _p('export PARTITION_NAME='+str(Online.PartitionName))
  _p('export RUN_TYPE='+str(Online.Activity))
  _p('export MOOREONLINE_VERSION='+str(Online.MooreOnlineVersion))
  _p('export MOORE_VERSION='+str(Online.MooreVersion))
  _p('export ONLINE_VERSION='+str(Online.OnlineVersion))
  _p('export OUTPUT_LEVEL='+str(Online.OutputLevel))
  _p('export MBM_SETUP_OPTIONS=/group/online/dataflow/options/'+str(Online.PartitionName)+'/HLT/MBM_setup.opts')
  if Online.TAE != 0:
    _p('export TAE_PROCESSING=TAE')
  if os.environ.has_key('CREATE_CHECKPOINT'):
    _p('export MOORESTARTUP_MODE=CREATE_CHECKPOINT')
  elif os.environ.has_key('TEST_CHECKPOINT'):
    _p('export MOORESTARTUP_MODE=RESTORE')
  elif Online.MooreStartupMode==0:
    _p('export MOORESTARTUP_MODE=NORMAL')
  elif Online.MooreStartupMode==1:
    _p('export MOORESTARTUP_MODE=FORKING')
  elif Online.MooreStartupMode==2:
    _p('export MOORESTARTUP_MODE=RESTORE')
  fn = '/group/online/dataflow/cmtuser/checkpoints/' + str(Online.HLTType) + '/' + \
       str(Online.OnlineVersion) + '/' + str(Online.HltArchitecture)
  _p('export CHECKPOINT_DIR='+fn)

except Exception,X:
  _p('echo '+str(X))
  _p('exit 11')

EOF`
#
#  Switch to the task directory and execute the controller process
#  ---------------------------------------------------------------
#### export ONLINE_VERSION=Online_v5r5;
if test "`uname -a | grep \".el6\"`" != "";   # SLC6
    then
    export CMTCONFIG=x86_64-slc6-gcc46-dbg;
    export CMTCONFIG=x86_64-slc6-gcc48-dbg;
    #export ONLINE_VERSION=Online_v5r8;
elif test "`uname -a | grep el5`" != "";      # SLC5
    then
    export CMTCONFIG=x86_64-slc5-gcc46-dbg;
fi;
. /group/online/dataflow/cmtuser/OnlineRelease/MyOnline/cmt/setup.${CMTCONFIG}.vars
cd ${FARMCONFIGROOT}/job;
##echo [ERROR] cd ${FARMCONFIGROOT}/job;
##cd /home/frankm/ONLINE/${ONLINE_VERSION}/Online/FarmConfig/job;




echo "===========> from runTask.sh"

if [ -r /etc/sysconfig/dim ]; then
   . /etc/sysconfig/dim
   export DIM_DNS_NODE=`echo $DIM_DNS_NODE | tr a-z A-Z`
fi
if [ -z $DIM_DNS_NODE ]; then
   echo DIM_DNS_NODE undefined and /etc/sysconfig/dim not readable
   # exit 1
fi;
#
export STATIC_OPTS=../options
export DYNAMIC_OPTS=/group/online/dataflow/options/${PARTITION_NAME}
export DATAINTERFACE=`python /group/online/dataflow/scripts/getDataInterface.py`
export ONLINETASKS=/group/online/dataflow/templates;
export INFO_OPTIONS=${DYNAMIC_OPTS}/${PARTITION_NAME}_Info.opts;
export SUBFARM_OPTIONS=${DYNAMIC_OPTS}/${PARTITION_NAME}_${DIM_DNS_NODE}_HLT.opts;
#
#
#
echo "===========>Executing preamble.sh"
. ./preamble.sh;
#. ./${TASK_TYPE}.sh;
