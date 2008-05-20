

set tmp2file = `SetupProject.py --shell=csh --mktemp ${*:q}`
set SetupProjectStatus = $?
if ( ! $SetupProjectStatus ) then
    source $tmp2file
endif
rm -f $tmp2file
unset tmp2file

