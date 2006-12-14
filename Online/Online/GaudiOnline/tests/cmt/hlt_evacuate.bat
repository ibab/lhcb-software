call %GAUDIONLINEROOT%\tests\cmt\preamble.bat
set msg_svc=MessageSvc
set UTGID=MBMMon
start "%UTGID%" %GAUDIONLINEROOT%\%CMTCONFIG%\Gaudi.exe OnlineKernel.dll mbm_mon
set UTGID=TanMon
start "%UTGID%" %GAUDIONLINEROOT%\%CMTCONFIG%\Gaudi.exe OnlineKernel.dll tanmon -c
set UTGID=OutputBuffer
start "%UTGID%" %gaudi_exe% -main=%OPTS%/MBMinit.opts -opt=%OPTS%/Daemon.opts -msgsvc=%msg_svc%
set UTGID=InputBuffer
start "%UTGID%" %gaudi_exe% -main=%OPTS%/MBMfarm.opts -opt=%OPTS%/Daemon.opts -msgsvc=%msg_svc%

sleep 3

set UTGID=Recv_0
start "%UTGID%" %gaudi_exe% -opt=%OPTS%/NetReceiver.opts  -msgsvc=%msg_svc%
sleep 2

set UTGID=Send0_0
start "%UTGID%" %gaudi_exe% -opt=%OPTS%/NetSend_0.opts    -msgsvc=%msg_svc% -debug
rem
set UTGID=Prod_0
rem start /LOW "%UTGID%" %GAUDIONLINEROOT%\%CMTCONFIG%\Gaudi.exe GaudiOnline mdf_producer -n=prod_0 -p=333 -s=500 -b=OUT_14d -f=./mdfdata_1.dat
rem start /LOW "%UTGID%" %gaudi_exe% -opt=%OPTS%/MDF2MBM.opts  -msgsvc=%msg_svc%
type %~dpnx0
