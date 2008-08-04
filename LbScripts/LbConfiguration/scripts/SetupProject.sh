
SetupProject_tmpfile=`SetupProject.py --shell=sh --mktemp "$@"`
SetupProjectStatus="$?"
if [ "$SetupProjectStatus" = 0 ]; then
    . $SetupProject_tmpfile
fi
rm -f $SetupProject_tmpfile
unset SetupProject_tmpfile
