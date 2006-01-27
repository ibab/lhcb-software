set PROCESSNAME=ALLOC_0
start ..\win32_vc71_dbg\test.exe tan_nameserver -a
set PROCESSNAME=INQUIRE_0
sleep 1
start ..\win32_vc71_dbg\test.exe tan_nameserver -i
set PROCESSNAME=ALLOC_TEST_0
start ..\win32_vc71_dbg\test.exe rtl_tan_alloc_test -c -q
rem start netstat -a -p UDP -v 5
sleep 2
set PROCESSNAME=INQUIRETST_0
start ..\win32_vc71_dbg\test.exe rtl_tan_inquire_test -c
set PROCESSNAME=TANMON_0
rem xterm -e "..\win32_vc71_dbg\test.exe tanmon -c"
rem sleep 3
set PROCESSNAME=TANDUMP
..\win32_vc71_dbg\test.exe rtl_tandb_dump
