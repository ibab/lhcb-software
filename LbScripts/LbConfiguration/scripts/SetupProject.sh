
tmpfile=`SetupProject.py --shell=sh --mktemp "$@"`
SetupProjectStatus="$?"
if [ "$SetupProjectStatus" = 0 ]; then
    . $tmpfile
fi
rm -f $tmpfile
unset tmpfile
