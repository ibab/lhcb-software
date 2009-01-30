scriptdir=$LBSCRIPTSPOLICYROOT/scripts

tmpfile=`runpy LbScriptsPolicy.PathStripper --shell=sh --mktemp -e PATH -e LD_LIBRARY_PATH -e PYTHONPATH -e JOBOPTSEARCHPATH -e HPATH `
. $tmpfile
rm -f $tmpfile
unset tmpfile
unset scriptdir
