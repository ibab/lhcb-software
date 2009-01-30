
set SetupProject_tmpfile = `runpy LbConfiguration.SetupProject --shell=csh --build-env --mktemp ${*:q}`
set SetupProjectStatus = $?
if ( ! $SetupProjectStatus ) then
    source $SetupProject_tmpfile
endif
rm -f $SetupProject_tmpfile
unset SetupProject_tmpfile

