#!/bin/csh
rm /dev/shm/sem.TAN_*
rm /dev/shm/TAN_*
set EXEC=$ONLINEKERNELROOT/$CMTCONFIG/test.exe
set MINITERM='xterm -geometry 80x12 -e '
set MIDITERM='xterm -geometry 100x12 -e '
$MINITERM "setenv PROCESSNAME TANALLOC_0;   $EXEC tan_nameserver -a" &
sleep 2
$MINITERM "setenv PROCESSNAME TANINQUIRE_0; $EXEC tan_nameserver -i -tcp" &
$MIDITERM "setenv PROCESSNAME ALLOCTST_0;   $EXEC rtl_tan_alloc_test -c -q" &
sleep 2
$MIDITERM "setenv PROCESSNAME INQUIRETST_0; $EXEC rtl_tan_inquire_test -c -q" &
sleep 2
xterm -e "setenv PROCESSNAME TANMON_0;      $EXEC tanmon -c" &
setenv PROCESSNAME TANDBDUMP_0; $EXEC rtl_tandb_dump

