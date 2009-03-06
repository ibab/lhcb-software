#!/bin/bash

#cd job
export DEBUGGING=YES
. ./setupOnline.sh $*

exec -a ${UTGID} ${gaudi_exe} -tasktype=LHCb::Class2Task -main=/group/online/dataflow/templates/options/Main.opts -opts=${HLTOPTS}/DiskWR${IS_TAE_RUN}.opts;
# to write files to local diskj on farm nodes:
# exec -a ${UTGID} ${gaudi_exe} -tasktype=LHCb::Class2Task -main=/group/online/dataflow/templates/options/Main.opts -opts=/home/online/Online_v4r12/Online/OnlineTasks/v1r13/options/OT_DiskWR.opts
