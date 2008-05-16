

set tmpfile = `SetupProject.py --shell=csh --mktemp ${*:q}`
set SetupProjectStatus = $?
if ( ! $SetupProjectStatus ) then
    source $tmpfile
endif
rm -f $tmpfile
unset tmpfile
