set PROCESSNAME=ALLOC_0
start ..\win32_vc71_dbg\test.exe tan_nameserver -a
set PROCESSNAME=INQUIRE_0
sleep 1
start ..\win32_vc71_dbg\test.exe tan_nameserver -i
set PROCESSNAME=ALLOC_TEST_0
start ..\win32_vc71_dbg\test.exe rtl_tan_alloc_test -c
rem start netstat -a -p UDP -v 5
sleep 3
set PROCESSNAME=TANDUMP
..\win32_vc71_dbg\test.exe tandb_dump
