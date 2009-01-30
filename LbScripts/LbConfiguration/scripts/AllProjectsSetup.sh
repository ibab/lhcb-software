
tmp2dffile=`runpy LbConfiguration.AllProjectsSetup --shell=sh --mktemp`
AllProjectsSetupStatus="$?"
if [ "$AllProjectsSetupStatus" = 0 ]; then
    . $tmp2dffile
fi
rm -f $tmp2dffile
unset tmp2dffile
unset AllProjectsSetupStatus
