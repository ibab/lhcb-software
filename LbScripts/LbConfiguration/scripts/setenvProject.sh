
SetupProject_tmpfile=`runpy LbConfiguration.SetupProject --shell=sh --build-env --mktemp "$@"`
SetupProjectStatus="$?"
if [ "$SetupProjectStatus" = 0 ]; then
    . $SetupProject_tmpfile
fi
rm -f $SetupProject_tmpfile
unset SetupProject_tmpfile
