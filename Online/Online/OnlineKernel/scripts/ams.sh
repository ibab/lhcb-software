#!/bin/csh
killall test.exe
export TAN_PORT=YES
set EXEC=$ONLINEKERNELROOT/$CMTCONFIG/test.exe
set MINITERM='xterm -geometry 80x12 -e '
set MIDITERM='xterm -geometry 100x12 -e '
$MINITERM "export PROCESSNAME=TANALLOC_0;   $EXEC tan_nameserver -a -d" &
sleep 1
# $MINITERM "export PROCESSNAME=TANINQUIRE_0; $EXEC tan_nameserver -i" &
sleep 1
$MINITERM "export PROCESSNAME=READER_0; $EXEC amsc_bounce -n=READER_0" &
sleep 2
#export PROCESSNAME=SENDER_0; gvd $EXEC &
#echo amsc_bounce -l=555 -s=READER_0 -n=SENDER_0" &
$MINITERM "export PROCESSNAME=SENDER_0; $EXEC amsc_bounce -l=555 -s=READER_0 -n=SENDER_0" &
$MINITERM "export PROCESSNAME=SENDER_1; $EXEC amsc_bounce -l=444 -s=READER_0 -n=SENDER_1" &
$MINITERM "export PROCESSNAME=SENDER_2; $EXEC amsc_bounce -l=333 -s=READER_0 -n=SENDER_2" &
sleep 2
xterm -e "export PROCESSNAME=TANMON_0;      $EXEC tanmon -c" &
