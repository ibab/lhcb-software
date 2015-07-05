#!/bin/bash
source /group/online/dataflow/cmtuser/DataQuality/setup.x86_64-slc6-gcc48-opt.vars;
export PYTHONPATH=${DATAQUALITYROOT}/python:$PYTHONPATH;
DATAQUALITY_UTILITIES()
{
  CFG=${1};
  shift;
  export DQ_CONFIG=${DATAQUALITYROOT}/python/DataQualityScan/${CFG}_Config.py;
  `which python` -c "import DataQualityScan.Utils as U; U.processArgs()" $*;
}
alias h2_util='DATAQUALITY_UTILITIES H2';
alias h2_reschedule='DATAQUALITY_UTILITIES H2 -r -d ';
alias h2_archive='DATAQUALITY_UTILITIES H2 -a -d';
alias h2_dump='DATAQUALITY_UTILITIES H2 -d';

alias dq_util='DATAQUALITY_UTILITIES DQ';
alias dq_reschedule='DATAQUALITY_UTILITIES DQ -r -d ';
alias dq_archive='DATAQUALITY_UTILITIES DQ -a -d';
alias dq_dump='DATAQUALITY_UTILITIES DQ -d';
