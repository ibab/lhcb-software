set PROCESSNAME=ALLOC_0
start %ONLINEKERNELROOT%\%CMTCONFIG%\gentest.exe OnlineKernel tan_nameserver -a
set PROCESSNAME=INQUIRE_0
sleep 1
start %ONLINEKERNELROOT%\%CMTCONFIG%\gentest.exe OnlineKernel tan_nameserver -i -tcp
sleep 1
set PROCESSNAME=TANMON_0
start %ONLINEKERNELROOT%\%CMTCONFIG%\gentest.exe OnlineKernel tanmon -c
set PROCESSNAME=TST_0
start %ONLINEKERNELROOT%\%CMTCONFIG%\gentest.exe UPIR upi_test
