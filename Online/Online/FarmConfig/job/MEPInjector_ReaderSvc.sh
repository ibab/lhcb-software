#!/bin/bash
export UTGID=${1};
. /group/online/dataflow/scripts/preamble.sh;
export HOST=$(hostname --short | awk '{ print toupper($1) }');
export READER_OPTS=`python -c "s='${UTGID}';print 'MEPInjector_'+s[s.find('_')+1:s.rfind('_')]+'.opts'"`;
#
echo "+++ Reader options:${READER_OPTS}";
exec -a ${UTGID}  ${gaudi_exe} -tasktype=LHCb::Class2Task \
    -main=${GAUDIONLINEROOT}/options/Main.opts \
    -opt=${FARMCONFIGROOT}/options/${READER_OPTS};
