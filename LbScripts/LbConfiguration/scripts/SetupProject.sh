
SetupProject_tmpfile=`runpy LbConfiguration.SetupProject --shell=sh --mktemp "$@"`
SetupProjectStatus="$?"
if [ "$SetupProjectStatus" = 0 -a -n "$SetupProject_tmpfile" ]; then
    . $SetupProject_tmpfile
fi
rm -f $SetupProject_tmpfile
unset SetupProject_tmpfile
