#!/bin/bash
export UTGID=${1};
. /group/online/dataflow/scripts/preamble.sh
export HOST=$(hostname --short | awk '{ print toupper($1) }')
#
exec -a ${UTGID}  ${gaudi_exe} -tasktype=LHCb::Class2Task \
    -main=${GAUDIONLINEROOT}/options/Main.opts \
    -opt=${FARMCONFIGROOT}/options/MEPInjector_ReaderSvc0.opts;
