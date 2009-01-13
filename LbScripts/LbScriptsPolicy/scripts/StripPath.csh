
set scriptdir=$LBSCRIPTSPOLICYROOT/scripts

set tmpfile = `python $scriptdir/PathStripper --shell=csh --mktemp -e PATH -e LD_LIBRARY_PATH -e PYTHONPATH -e JOBOPTSEARCHPATH -e HPATH `
source $tmpfile
rm -f $tmpfile
unset tmpfile
unset scriptdir

