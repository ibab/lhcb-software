
set SetupProject_tmpfile = `runpy LbConfiguration.SetupProject --shell=csh --mktemp ${*:q}`
set SetupProjectStatus = $?
if ( ! $SetupProjectStatus && "$SetupProject_tmpfile" != "" ) then
    source $SetupProject_tmpfile
endif
rm -f $SetupProject_tmpfile
unset SetupProject_tmpfile

