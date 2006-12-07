call %GAUDIONLINEROOT%\tests\cmt\preamble.bat
set UTGID=MBMMon
start "%UTGID%" %GAUDIONLINEROOT%\%CMTCONFIG%\Gaudi.exe OnlineKernel.dll mbm_mon
set UTGID=MEPInit
start "%UTGID%" %gaudi_exe% -main=%OPTS%/MEPinit.opts -opt=%OPTS%/Daemon.opts -msgsvc=%msg_svc%
set UTGID=ErrServ
start "%UTGID%" %gaudi_exe% -opt=%OPTS%/ErrorSrv.opts     -msgsvc=%msg_svc%
set UTGID=ErrLog
start "%UTGID%" %gaudi_exe% -opt=%OPTS%/ErrorLogger.opts  -msgsvc=MessageSvc

sleep 5
set UTGID=EvtProd
start "%UTGID%" %gaudi_exe% -opt=%OPTS%/MEPConverter.opts -msgsvc=%msg_svc%
set UTGID=EvtHolder
start "%UTGID%" %gaudi_exe% -opt=%OPTS%/MEPHolder.opts    -msgsvc=%msg_svc%
set UTGID=Moore_0
start "%UTGID%" %gaudi_exe% -opt=%OPTS%/ReadMBM.opts      -msgsvc=%msg_svc%
rem
rem
rem start "prod_0" %GAUDIONLINEROOT%\%CMTCONFIG%\Gaudi.exe GaudiOnline mep_producer -n=prod_0 -p=333 -s=500 -r=2
type %~dpnx0
