
set tmp2dffile = `python -m LbConfiguration.AllProjectsSetup --shell=csh --mktemp`
set AllProjectsSetupStatus = $?
if ( ! $AllProjectsSetupStatus ) then
    source $tmp2dffile
endif
rm -f $tmp2dffile
unset tmp2dffile
unset AllProjectsSetupStatus

