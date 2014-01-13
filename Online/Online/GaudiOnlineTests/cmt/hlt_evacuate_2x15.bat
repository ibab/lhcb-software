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
set UTGID=ErrSrv
rem start "%UTGID%" %gaudi_exe% -opt=%OPTS%/ErrorSrv.opts     -msgsvc=%msg_svc%
set UTGID=ErrLog
rem start "%UTGID%" %gaudi_exe% -opt=%OPTS%/ErrorLogger.opts  -msgsvc=MessageSvc

sleep 3
set UTGID=Recv_0
start "%UTGID%" %gaudi_exe% -opt=%OPTS%/NetReceiver.opts  -msgsvc=%msg_svc%
set UTGID=Recv_1
start "%UTGID%" %gaudi_exe% -opt=%OPTS%/NetReceiver.opts  -msgsvc=%msg_svc%
sleep 4
set UTGID=Send0_0
start "%UTGID%" %gaudi_exe% -opt=%OPTS%/NetSend_0.opts    -msgsvc=%msg_svc%
set UTGID=Send0_1
start "%UTGID%" %gaudi_exe% -opt=%OPTS%/NetSend_0.opts    -msgsvc=%msg_svc%
set UTGID=Send0_2
start "%UTGID%" %gaudi_exe% -opt=%OPTS%/NetSend_0.opts    -msgsvc=%msg_svc%
set UTGID=Send0_3
start "%UTGID%" %gaudi_exe% -opt=%OPTS%/NetSend_0.opts    -msgsvc=%msg_svc%
set UTGID=Send0_4
start "%UTGID%" %gaudi_exe% -opt=%OPTS%/NetSend_0.opts    -msgsvc=%msg_svc%
set UTGID=Send0_5
start "%UTGID%" %gaudi_exe% -opt=%OPTS%/NetSend_0.opts    -msgsvc=%msg_svc%
set UTGID=Send0_6
start "%UTGID%" %gaudi_exe% -opt=%OPTS%/NetSend_0.opts    -msgsvc=%msg_svc%
set UTGID=Send0_7
start "%UTGID%" %gaudi_exe% -opt=%OPTS%/NetSend_0.opts    -msgsvc=%msg_svc%
set UTGID=Send0_8
start "%UTGID%" %gaudi_exe% -opt=%OPTS%/NetSend_0.opts    -msgsvc=%msg_svc%
set UTGID=Send0_9
start "%UTGID%" %gaudi_exe% -opt=%OPTS%/NetSend_0.opts    -msgsvc=%msg_svc%
set UTGID=Send0_10
start "%UTGID%" %gaudi_exe% -opt=%OPTS%/NetSend_0.opts    -msgsvc=%msg_svc%
set UTGID=Send0_11
start "%UTGID%" %gaudi_exe% -opt=%OPTS%/NetSend_0.opts    -msgsvc=%msg_svc%
set UTGID=Send0_12
start "%UTGID%" %gaudi_exe% -opt=%OPTS%/NetSend_0.opts    -msgsvc=%msg_svc%
set UTGID=Send0_13
start "%UTGID%" %gaudi_exe% -opt=%OPTS%/NetSend_0.opts    -msgsvc=%msg_svc%
set UTGID=Send0_14
start "%UTGID%" %gaudi_exe% -opt=%OPTS%/NetSend_0.opts    -msgsvc=%msg_svc%

set UTGID=Send1_0
start "%UTGID%" %gaudi_exe% -opt=%OPTS%/NetSend_1.opts    -msgsvc=%msg_svc%
set UTGID=Send1_1
start "%UTGID%" %gaudi_exe% -opt=%OPTS%/NetSend_1.opts    -msgsvc=%msg_svc%
set UTGID=Send1_2
start "%UTGID%" %gaudi_exe% -opt=%OPTS%/NetSend_1.opts    -msgsvc=%msg_svc%
set UTGID=Send1_3
start "%UTGID%" %gaudi_exe% -opt=%OPTS%/NetSend_1.opts    -msgsvc=%msg_svc%
set UTGID=Send1_4
start "%UTGID%" %gaudi_exe% -opt=%OPTS%/NetSend_1.opts    -msgsvc=%msg_svc%
set UTGID=Send1_5
start "%UTGID%" %gaudi_exe% -opt=%OPTS%/NetSend_1.opts    -msgsvc=%msg_svc%
set UTGID=Send1_6
start "%UTGID%" %gaudi_exe% -opt=%OPTS%/NetSend_1.opts    -msgsvc=%msg_svc%
set UTGID=Send1_7
start "%UTGID%" %gaudi_exe% -opt=%OPTS%/NetSend_1.opts    -msgsvc=%msg_svc%
set UTGID=Send1_8
start "%UTGID%" %gaudi_exe% -opt=%OPTS%/NetSend_1.opts    -msgsvc=%msg_svc%
set UTGID=Send1_9
start "%UTGID%" %gaudi_exe% -opt=%OPTS%/NetSend_1.opts    -msgsvc=%msg_svc%
set UTGID=Send1_10
start "%UTGID%" %gaudi_exe% -opt=%OPTS%/NetSend_1.opts    -msgsvc=%msg_svc%
set UTGID=Send1_11
start "%UTGID%" %gaudi_exe% -opt=%OPTS%/NetSend_1.opts    -msgsvc=%msg_svc%
set UTGID=Send1_12
start "%UTGID%" %gaudi_exe% -opt=%OPTS%/NetSend_1.opts    -msgsvc=%msg_svc%
set UTGID=Send1_13
start "%UTGID%" %gaudi_exe% -opt=%OPTS%/NetSend_1.opts    -msgsvc=%msg_svc%
set UTGID=Send1_14
start "%UTGID%" %gaudi_exe% -opt=%OPTS%/NetSend_0.opts    -msgsvc=%msg_svc%

rem start /b xterm -132 -geometry 132x45  -title ErrorLogger  -e /bin/bash -l -c "export UTGID=ErrorLogger; cd J:/Gaudi/Online/UPI/cmt; ./upi.sh ../../GaudiOnline/%CMTCONFIG%/gaudi.exe ../../GaudiOnline/%CMTCONFIG%/GaudiOnline OnlineTask -opts=../../GaudiUPI/options/Errlog.opts -msgsvc=%msg_svc%"
rem
set UTGID=Prod_0
rem start /LOW "%UTGID%" %GAUDIONLINEROOT%\%CMTCONFIG%\Gaudi.exe GaudiOnline mdf_producer -n=prod_0 -p=333 -s=500 -b=OUT_14d -f=./mdfdata_1.dat
rem start /LOW "%UTGID%" %gaudi_exe% -opt=%OPTS%/MDF2MBM.opts  -msgsvc=%msg_svc%
type %~dpnx0
