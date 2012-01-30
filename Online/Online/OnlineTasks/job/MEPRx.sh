#!/bin/bash
export DEBUGGING=true
if test "$2" = "LHCb1";
    then
    . ./overflowreader.sh
fi;
. ./setupOnline.sh $*
export TELL1BOARDS=/group/online/dataflow/options/${PARTNAME}/${PARTNAME}_Tell1Boards.opts
export LOGFIFO=/tmp/logGaudi.fifo # markus & niko 20/12/11
#
#
exec -a ${UTGID} ${gaudi_exe} -tasktype=LHCb::Class2Task -main=/group/online/dataflow/templates/options/Main.opts -opt=${HLTOPTS}/MEPRxSvc.opts
