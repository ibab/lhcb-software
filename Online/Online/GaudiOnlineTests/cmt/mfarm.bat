call %GAUDIONLINEROOT%\tests\cmt\preamble.bat
set msg_svc=MessageSvc
set UTGID=MBMMon
start "%UTGID%" %GAUDIONLINEROOT%\%CMTCONFIG%\Gaudi.exe OnlineKernel.dll mbm_mon
set UTGID=TanMon
rem start "%UTGID%" %GAUDIONLINEROOT%\%CMTCONFIG%\Gaudi.exe OnlineKernel.dll tanmon -c
set UTGID=OutputBuffer
start "%UTGID%" %gaudi_exe% -main=%OPTS%/MBMinit.opts -opt=%OPTS%/Daemon.opts -msgsvc=%msg_svc%
set UTGID=InputBuffer
start "%UTGID%" %gaudi_exe% -main=%OPTS%/MBMfarm.opts -opt=%OPTS%/Daemon.opts -msgsvc=%msg_svc%
set UTGID=ErrSrv
rem start "%UTGID%" %gaudi_exe% -opt=%OPTS%/ErrorSrv.opts     -msgsvc=%msg_svc%
set UTGID=ErrLog
rem start "%UTGID%" %gaudi_exe% -opt=%OPTS%/ErrorLogger.opts  -msgsvc=MessageSvc

sleep 4
set UTGID=Mon_0
start "%UTGID%" %gaudi_exe% -opt=%OPTS%/Monitor.opts      -msgsvc=%msg_svc%
rem
set UTGID=Sender
start "%UTGID%" %gaudi_exe% -opt=%OPTS%/FarmSupply.opts    -msgsvc=%msg_svc%
sleep 3
set UTGID=Receiver
start "%UTGID%" %gaudi_exe% -opt=%OPTS%/FarmReceiver.opts  -msgsvc=%msg_svc% 

rem start /b xterm -132 -geometry 132x45  -title ErrorLogger  -e /bin/bash -l -c "export UTGID=ErrorLogger; cd J:/Gaudi/Online/UPI/cmt; ./upi.sh ../../GaudiOnline/%CMTCONFIG%/gaudi.exe ../../GaudiOnline/%CMTCONFIG%/GaudiOnline OnlineTask -opts=../../GaudiUPI/options/Errlog.opts -msgsvc=%msg_svc%"
rem
set UTGID=Prod_0
rem start /LOW "%UTGID%" %GAUDIONLINEROOT%\%CMTCONFIG%\Gaudi.exe GaudiOnline mdf_producer -n=prod_0 -p=333 -s=500 -b=OUT_14d -f=./mdfdata_1.dat
rem start /LOW "%UTGID%" %gaudi_exe% -opt=%OPTS%/MDF2MBM.opts  -msgsvc=%msg_svc%
type %~dpnx0
