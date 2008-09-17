
set LbLogin_tmpfile = `LbLogin.py --shell=csh --mktemp ${*:q}`
set LbLoginStatus = $?
if ( ! $LbLoginStatus ) then
    source $LbLogin_tmpfile
endif
rm -f $LbLogin_tmpfile
unset LbLogin_tmpfile

