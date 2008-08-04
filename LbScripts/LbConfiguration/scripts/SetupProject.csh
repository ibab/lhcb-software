
set SetupProject_tmpfile = `SetupProject.py --shell=csh --mktemp ${*:q}`
set SetupProjectStatus = $?
if ( ! $SetupProjectStatus ) then
    source $SetupProject_tmpfile
endif
rm -f $SetupProject_tmpfile
unset SetupProject_tmpfile

