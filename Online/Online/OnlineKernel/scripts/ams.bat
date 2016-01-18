set EXEC=START %ONLINEKERNELROOT%\%CMTCONFIG%\test.exe
set PROCESSNAME=TANALLOC_0   &&  %EXEC% tan_nameserver -a
sleep 1
set PROCESSNAME=TANINQUIRE_0 && %EXEC% tan_nameserver -i -tcp
sleep 1
set PROCESSNAME=READER_0 && %EXEC% amsc_bounce -n=READER_0
sleep 1
set PROCESSNAME=READER_0 && %EXEC% amsc_bounce -l=555 -s=READER_0 -n=SENDER_0
set PROCESSNAME=READER_0 && %EXEC% amsc_bounce -l=444 -s=READER_0 -n=SENDER_1
set PROCESSNAME=READER_0 && %EXEC% amsc_bounce -l=333 -s=READER_0 -n=SENDER_2
sleep 2
set PROCESSNAME=TANMON_0 && %EXEC% tanmon -c
