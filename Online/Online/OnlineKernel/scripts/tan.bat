set PROCESSNAME=ALLOC_0
start %ONLINEKERNELROOT%\%CMTCONFIG%\test.exe tan_nameserver -a
set PROCESSNAME=INQUIRE_0
sleep 1
start %ONLINEKERNELROOT%\%CMTCONFIG%\test.exe tan_nameserver -i -tcp
set PROCESSNAME=ALLOC_TEST_0
start %ONLINEKERNELROOT%\%CMTCONFIG%\test.exe rtl_tan_alloc_test -c -q
rem start netstat -a -p UDP -v 5
sleep 2
set PROCESSNAME=INQUIRETST_0
start %ONLINEKERNELROOT%\%CMTCONFIG%\test.exe rtl_tan_inquire_test -c
set PROCESSNAME=TANMON_0
rem xterm -e "%ONLINEKERNELROOT%\%CMTCONFIG%\test.exe tanmon -c"
rem sleep 3
set PROCESSNAME=TANDUMP
%ONLINEKERNELROOT%\%CMTCONFIG%\test.exe rtl_tandb_dump
