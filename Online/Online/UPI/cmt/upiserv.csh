#/bin/csh
rm /dev/shm/sem.TAN_*
rm /dev/shm/TAN_*
set EXEC=$ONLINEKERNELROOT/$CMTCONFIG/gentest.exe
set MINITERM='xterm -geometry 80x12 -e '
set MIDITERM='xterm -geometry 100x12 -e '
$MINITERM "setenv PROCESSNAME TANALLOC_0;   $EXEC libOnlineKernel.so tan_nameserver -a" &
sleep 2
$MINITERM "setenv PROCESSNAME TANINQUIRE_0; $EXEC libOnlineKernel.so tan_nameserver -i -tcp" &
sleep 2
xterm -e "setenv PROCESSNAME TANMON_0;      $EXEC libOnlineKernel.so tanmon -c" &
xterm -e "setenv PROCESSNAME TST_0;         $EXEC libUPIR.so upi_test" &
xterm -e "setenv PROCESSNAME OTHER_0;       $EXEC libUPIR.so upi_test" &
