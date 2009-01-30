
set tmp2dffile = `runpy LbConfiguration.AllProjectsSetup --shell=csh --mktemp`
set AllProjectsSetupStatus = $?
if ( ! $AllProjectsSetupStatus ) then
    source $tmp2dffile
endif
rm -f $tmp2dffile
unset tmp2dffile
unset AllProjectsSetupStatus

