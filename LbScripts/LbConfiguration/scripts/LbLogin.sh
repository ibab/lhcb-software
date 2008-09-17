LbLogin_tmpfile=`LbLogin.py --shell=sh --mktemp "$@"`
LbLoginStatus="$?"
if [ "$LbLoginStatus" = 0 ]; then
    . $LbLogin_tmpfile
fi
rm -f $LbLogin_tmpfile
unset LbLogin_tmpfile
