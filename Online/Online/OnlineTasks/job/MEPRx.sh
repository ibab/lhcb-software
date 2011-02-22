#!/bin/bash
export DEBUGGING=true
. ./setupOnline.sh $*
export TELL1BOARDS=/group/online/dataflow/options/${PARTNAME}/${PARTNAME}_Tell1Boards.opts
exec -a ${UTGID} ${gaudi_exe} -tasktype=LHCb::Class2Task -main=/group/online/dataflow/templates/options/Main.opts -opt=${HLTOPTS}/MEPRxSvc.opts
