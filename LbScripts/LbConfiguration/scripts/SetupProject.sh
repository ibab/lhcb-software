
tmp2file=`SetupProject.py --shell=sh --mktemp "$@"`
SetupProjectStatus="$?"
if [ "$SetupProjectStatus" = 0 ]; then
    . $tmp2file
fi
rm -f $tmp2file
unset tmp2file
