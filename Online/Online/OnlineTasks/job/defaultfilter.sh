#!/bin/bash
# Call Moore startup script
# should be in /group/hlt/MOORE/Moore_version/Hlt/Moore/job/runMooreOnline_EFF.sh
# $1: Moore_vxry
# $2: Subfarmname
# $3: Partitionname
# $4: Runtype
export PYTHONPATH=/group/online/dataflow/options/${3}/HLT:${PYTHONPATH}
RunType=`python -c "import OnlineEnvBase;print OnlineEnvBase.HLTType;"`
if test "${RunType}" = "PassThrough";
    then
    . ./passthrough.sh
fi;
. /group/hlt/MOORE/$1/Hlt/Moore/job/runMooreOnline_EFF.sh $2 $3 $4
