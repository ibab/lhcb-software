set test_exe=%ONLINEKERNELROOT%\%CMTCONFIG%\gentest.exe
set gaudi_exe=%GAUDIONLINEROOT%\%CMTCONFIG%\Gaudi.exe
set PROCESSNAME=ALLOC_0
start %ONLINEKERNELROOT%\%CMTCONFIG%\gentest.exe OnlineKernel tan_nameserver -a
set PROCESSNAME=INQUIRE_0
sleep 1
start %ONLINEKERNELROOT%\%CMTCONFIG%\gentest.exe OnlineKernel tan_nameserver -i -tcp
sleep 1
set PROCESSNAME=TANMON_0
start %ONLINEKERNELROOT%\%CMTCONFIG%\gentest.exe OnlineKernel tanmon -c
set PROCESSNAME=TST_0
copy /Y %UPIROOT%\%CMTCONFIG%\UPIR.dll UPI.dll
start %gaudi_exe% GaudiOnline.dll OnlineTask -optopt=..\options\MEPRx.opts
cd %UPIROOT%/cmt
start /b xterm +nul -132 -im -sb -sl 500 -n UpiServer -title UpiServer -geometry 132x45 -e /bin/bash ../../UPI/cmt/upi.sh  gentest.exe UPI upi_server
cd %GAUDIONLINEROOT%/cmt
