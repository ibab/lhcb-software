#!/bin/bash
# =========================================================================
#
#  Default script to start the event reader task on the HLT farm
#
#  Author   M.Frank
#  Version: 1.0
#  Date:    20/05/2013
#
# =========================================================================
#
#!/bin/bash
. /group/online/dataflow/scripts/preamble.sh;
export ONLINE_VERSION=`python -c "import os;s=os.environ['DIMROOT'];i=s.find('/ONLINE_');print s[i+1:i+1+s[i+1:].find('/')]"`;
#
TASKCLASS=-tasktype=LHCb::${2}Task;
TASKTYPE=${3};
export DIM_DNS_NODE=${4};
DYNAMIC_OPTS=/group/online/dataflow/options;
OPTIONS=${DYNAMIC_OPTS}/${PARTITION}/${1};
#
#
#export PREAMBLE_OPTS=${ONLINETASKS}/options/Preamble.opts;
export ONLINE_ENV_DIR=/group/online/dataflow/options/${PARTITION}/MONITORING
export PYTHONPATH=${ONLINE_ENV_DIR}:${PYTHONPATH}
export AdderOptsFile=/tmp/${PARTITION}_MonAdderOpts.opts;
export AdderStaticOpts=../options/NodeAdderStatic.opts;
cd /group/online/dataflow/cmtuser/OnlineRelease/Online/FarmConfig/job;
python ./MonAdderOpts.py ${AdderOptsFile}
#
###export LD_LIBRARY_PATH=/home/beat/cmtuser/myOnline/InstallArea/${CMTCONFIG}/lib:$LD_LIBRARY_PATH
##echo $Class1_task
export INFO_OPTIONS=/group/online/dataflow/options/${PARTITION}/${PARTITION}_Info.opts
#
exec -a ${UTGID} $gaudi_exe ${TASKCLASS} -opt=../options/genAdder.opts -main=${ONLINETASKS}/options/Main.opts

