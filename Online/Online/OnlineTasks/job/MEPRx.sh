#!/bin/bash
export DEBUGGING=true
#echo "[ERROR] ${PARTNAME}"${PARTNAME} $*
if test "${2}" = "LHCb1";
    then
    . ./overflowreader.sh
fi;
. ./setupOnline.sh $*
export TELL1BOARDS=/group/online/dataflow/options/${PARTNAME}/${PARTNAME}_Tell1Boards.opts
export LOGFIFO=/tmp/logGaudi.fifo # markus & niko 20/12/11
#
#
exec -a ${UTGID} ${gaudi_exe} -tasktype=LHCb::Class2Task -main=/group/online/dataflow/templates/options/Main.opts -opt=${HLTOPTS}/MEPRxSvc.opts

##echo "[ERROR] Running activity: `grep Activity /group/online/dataflow/options/${PARTNAME}/HLT/OnlineEnvBase.py | grep EOF_CALIB`";
##if test -n "`grep Activity /group/online/dataflow/options/${PARTNAME}/HLT/OnlineEnvBase.py|grep EOF_CALIB`";
##    then
##    exec -a ${UTGID} ${gaudi_exe} -tasktype=LHCb::Class2Task -main=/group/online/dataflow/templates/options/Main.opts -opt=${HLTOPTS}/MEPRxSvc_EoFCalib.opts
##else
##    exec -a ${UTGID} ${gaudi_exe} -tasktype=LHCb::Class2Task -main=/group/online/dataflow/templates/options/Main.opts -opt=${HLTOPTS}/MEPRxSvc.opts
##fi;
