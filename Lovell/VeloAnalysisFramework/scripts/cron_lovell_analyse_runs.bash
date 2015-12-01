#!/bin/bash --login

# for debugging
# set -o xtrace

# setup
declare script=$(readlink -e $BASH_SOURCE)
declare srcdir="${script%/*}"

# save job arguments
declare jobargs="$@"
shift $#

# initialise environment; LbLogin et al.
source /cvmfs/lhcb.cern.ch/group_login.sh &> /dev/null
echo ${LBSCRIPTS_HOME}

# set resource limits
ulimit -Sm 2500000
ulimit -Sv 2500000
echo "Running on:" $HOSTNAME

# run job
exec cat /calib/velo/dqm/VeloView/VetraOutput/RunList_Merged_NZS_Clusters_Brunel.txt | xargs /calib/velo/dqm/lhcb-prerelease/Today/LOVELL/LOVELL_HEAD/build.x86_64-slc6-gcc48-opt/run python /calib/velo/dqm/lhcb-prerelease/Today/LOVELL/LOVELL_HEAD/VeloAnalysisFramework/python/veloview/analysis/analyse_run.py
