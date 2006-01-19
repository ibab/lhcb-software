#/bin/csh
#rm /dev/shm/bm_*
set EXEC=$ONLINEKERNELROOT/$CMTCONFIG/test.exe
set MINITERM='xterm -geometry 80x12 -e '
$MINITERM "setenv PROCESSNAME TANALLOC_0;   $EXEC tan_nameserver -a" &
sleep 2
$MINITERM "setenv PROCESSNAME TANINQUIRE_0; $EXEC tan_nameserver -i" &
xterm -e  "setenv PROCESSNAME ALLOCTST_0;   $EXEC rtl_tan_alloc_test -c" &
sleep 3
setenv PROCESSNAME TANDBDUMP_0
$EXEC rtl_tandb_dump

