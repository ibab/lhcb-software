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
set UTGID=ErrServ
rem start "%UTGID%" %gaudi_exe% -opt=%OPTS%/ErrorSrv.opts     -msgsvc=%msg_svc%
set UTGID=ErrLog
rem start "%UTGID%" %gaudi_exe% -opt=%OPTS%/ErrorLogger.opts  -msgsvc=MessageSvc

sleep 2

set UTGID=Recv_0
start "%UTGID%" %gaudi_exe% -opt=%OPTS%/NetReceiver.opts  -msgsvc=%msg_svc%
set UTGID=Recv_1
start "%UTGID%" %gaudi_exe% -opt=%OPTS%/NetReceiver.opts  -msgsvc=%msg_svc%
set UTGID=Recv_2
start "%UTGID%" %gaudi_exe% -opt=%OPTS%/NetReceiver.opts  -msgsvc=%msg_svc%
set UTGID=Recv_3
start "%UTGID%" %gaudi_exe% -opt=%OPTS%/NetReceiver.opts  -msgsvc=%msg_svc%
set UTGID=Recv_4
start "%UTGID%" %gaudi_exe% -opt=%OPTS%/NetReceiver.opts  -msgsvc=%msg_svc%
set UTGID=Recv_5
start "%UTGID%" %gaudi_exe% -opt=%OPTS%/NetReceiver.opts  -msgsvc=%msg_svc%
sleep 3

set UTGID=Send0_0
start "%UTGID%" %gaudi_exe% -opt=%OPTS%/NetSend_0.opts    -msgsvc=%msg_svc%
set UTGID=Send0_1
start "%UTGID%" %gaudi_exe% -opt=%OPTS%/NetSend_0.opts    -msgsvc=%msg_svc%
set UTGID=Send0_2
start "%UTGID%" %gaudi_exe% -opt=%OPTS%/NetSend_0.opts    -msgsvc=%msg_svc%
set UTGID=Send0_3
start "%UTGID%" %gaudi_exe% -opt=%OPTS%/NetSend_0.opts    -msgsvc=%msg_svc%

set UTGID=Send1_0
start "%UTGID%" %gaudi_exe% -opt=%OPTS%/NetSend_1.opts    -msgsvc=%msg_svc%
set UTGID=Send1_1
start "%UTGID%" %gaudi_exe% -opt=%OPTS%/NetSend_1.opts    -msgsvc=%msg_svc%
set UTGID=Send1_2
start "%UTGID%" %gaudi_exe% -opt=%OPTS%/NetSend_1.opts    -msgsvc=%msg_svc%
set UTGID=Send1_3
start "%UTGID%" %gaudi_exe% -opt=%OPTS%/NetSend_1.opts    -msgsvc=%msg_svc%

set UTGID=Send2_0
start "%UTGID%" %gaudi_exe% -opt=%OPTS%/NetSend_2.opts    -msgsvc=%msg_svc%
set UTGID=Send2_1
start "%UTGID%" %gaudi_exe% -opt=%OPTS%/NetSend_2.opts    -msgsvc=%msg_svc%
set UTGID=Send2_2
start "%UTGID%" %gaudi_exe% -opt=%OPTS%/NetSend_2.opts    -msgsvc=%msg_svc%
set UTGID=Send2_3
start "%UTGID%" %gaudi_exe% -opt=%OPTS%/NetSend_2.opts    -msgsvc=%msg_svc%

set UTGID=Send3_0
start "%UTGID%" %gaudi_exe% -opt=%OPTS%/NetSend_3.opts    -msgsvc=%msg_svc%
set UTGID=Send3_1
start "%UTGID%" %gaudi_exe% -opt=%OPTS%/NetSend_3.opts    -msgsvc=%msg_svc%
set UTGID=Send3_2
start "%UTGID%" %gaudi_exe% -opt=%OPTS%/NetSend_3.opts    -msgsvc=%msg_svc%
set UTGID=Send3_3
start "%UTGID%" %gaudi_exe% -opt=%OPTS%/NetSend_3.opts    -msgsvc=%msg_svc%

set UTGID=Send4_0
start "%UTGID%" %gaudi_exe% -opt=%OPTS%/NetSend_4.opts    -msgsvc=%msg_svc%
set UTGID=Send4_1
start "%UTGID%" %gaudi_exe% -opt=%OPTS%/NetSend_4.opts    -msgsvc=%msg_svc%
set UTGID=Send4_2
start "%UTGID%" %gaudi_exe% -opt=%OPTS%/NetSend_4.opts    -msgsvc=%msg_svc%
set UTGID=Send4_3
start "%UTGID%" %gaudi_exe% -opt=%OPTS%/NetSend_4.opts    -msgsvc=%msg_svc%

set UTGID=Send5_0
start "%UTGID%" %gaudi_exe% -opt=%OPTS%/NetSend_5.opts    -msgsvc=%msg_svc%
set UTGID=Send5_1
start "%UTGID%" %gaudi_exe% -opt=%OPTS%/NetSend_5.opts    -msgsvc=%msg_svc%
set UTGID=Send5_2
start "%UTGID%" %gaudi_exe% -opt=%OPTS%/NetSend_5.opts    -msgsvc=%msg_svc%
set UTGID=Send5_3
start "%UTGID%" %gaudi_exe% -opt=%OPTS%/NetSend_5.opts    -msgsvc=%msg_svc%

rem start /b xterm -132 -geometry 132x45  -title ErrorLogger  -e /bin/bash -l -c "export UTGID=ErrorLogger; cd J:/Gaudi/Online/UPI/cmt; ./upi.sh ../../GaudiOnline/%CMTCONFIG%/gaudi.exe ../../GaudiOnline/%CMTCONFIG%/GaudiOnline OnlineTask -opts=../../GaudiUPI/options/Errlog.opts -msgsvc=%msg_svc%"
rem
set UTGID=Prod_0
rem start /LOW "%UTGID%" %GAUDIONLINEROOT%\%CMTCONFIG%\Gaudi.exe GaudiOnline mdf_producer -n=prod_0 -p=333 -s=500 -b=OUT_14d -f=./mdfdata_1.dat
rem start /LOW "%UTGID%" %gaudi_exe% -opt=%OPTS%/MDF2MBM.opts  -msgsvc=%msg_svc%
type %~dpnx0
