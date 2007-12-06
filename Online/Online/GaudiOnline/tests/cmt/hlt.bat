call %GAUDIONLINEROOT%\tests\cmt\preamble.bat
set UTGID=MBMMon
start "%UTGID%" %GAUDIONLINEROOT%\%CMTCONFIG%\Gaudi.exe OnlineKernel.dll mbm_mon
set UTGID=TanServ
start "%UTGID%" %GAUDIONLINEROOT%\%CMTCONFIG%\Gaudi.exe OnlineKernel.dll tan_nameserver -a -tcp -d
set UTGID=MEPInit
rem start "%UTGID%" %gaudi_exe% -main=%OPTS%/MEPinit.opts -opt=%OPTS%/Daemon.opts    -msgsvc=%msg_svc%
start "%UTGID%" %gaudi_exe% -opt=%OPTS%/MEPinit.opts -main=%OPTS%/Main.opts    -msgsvc=%msg_svc%
set UTGID=OutputBuffer
start "%UTGID%" %gaudi_exe% -opt=%OPTS%/MBMinit.opts -main=%OPTS%/Main.opts    -msgsvc=%msg_svc%
set UTGID=ErrSrv
rem start "%UTGID%" %gaudi_exe% -main=%OPTS%/Main.opts -opt=%OPTS%/ErrorSrv.opts     -msgsvc=%msg_svc%
set UTGID=ErrLog
rem start "%UTGID%" %gaudi_exe% -main=%OPTS%/Main.opts -opt=%OPTS%/ErrorLogger.opts  -msgsvc=MessageSvc
sleep 10
set UTGID=EvtProd
start "%UTGID%" %gaudi_exe% -main=%OPTS%/Main.opts -opt=%OPTS%/MEPConverter.opts -msgsvc=%msg_svc%
set UTGID=EvtHolder
start "%UTGID%" %gaudi_exe% -main=%OPTS%/Main.opts -opt=%OPTS%/MEPHolder.opts    -msgsvc=%msg_svc%
set UTGID=Moore_0
start "%UTGID%" %gaudi_exe% -main=%OPTS%/Main.opts -opt=%OPTS%/ReadMBM.opts      -msgsvc=%msg_svc%
set UTGID=Moore_1
start "%UTGID%" %gaudi_exe% -main=%OPTS%/Main.opts -opt=%OPTS%/ReadMBM.opts      -msgsvc=%msg_svc%
set UTGID=Moore_2
start "%UTGID%" %gaudi_exe% -main=%OPTS%/Main.opts -opt=%OPTS%/ReadMBM.opts      -msgsvc=%msg_svc%
set UTGID=Spy_0
start "%UTGID%" %gaudi_exe% -main=%OPTS%/Main.opts -opt=%OPTS%/SpyMBMSlow.opts   -msgsvc=%msg_svc%
set UTGID=Spy_1
start "%UTGID%" %gaudi_exe% -main=%OPTS%/Main.opts -opt=%OPTS%/SpyMBM.opts       -msgsvc=%msg_svc%
set UTGID=Spy_2
start "%UTGID%" %gaudi_exe% -main=%OPTS%/Main.opts -opt=%OPTS%/SpyMBMFast.opts   -msgsvc=%msg_svc%
rem
sleep 2
set UTGID=Receiver
start "%UTGID%" %gaudi_exe% -main=%OPTS%/Main.opts -opt=%OPTS%/MDFReceiver.opts  -msgsvc=%msg_svc%
sleep 2
set UTGID=Sender
start "%UTGID%" %gaudi_exe% -main=%OPTS%/Main.opts -opt=%OPTS%/MDFSender.opts    -msgsvc=%msg_svc%
set UTGID=DiskWR
start "%UTGID%" %gaudi_exe% -main=%OPTS%/Main.opts -opt=%OPTS%/DiskWR.opts       -msgsvc=%msg_svc%
set UTGID=MDFWriter
rem start "%UTGID%" %gaudi_exe% -main=%OPTS%/Main.opts -opt=%OPTS%/MDFWriter.opts    -msgsvc=%msg_svc%

rem start /b xterm -132 -geometry 132x45  -title ErrorLogger  -e /bin/bash -l -c "export UTGID=ErrorLogger; cd J:/Gaudi/Online/UPI/cmt; ./upi.sh ../../GaudiOnline/%CMTCONFIG%/gaudi.exe ../../GaudiOnline/%CMTCONFIG%/GaudiOnline OnlineTask -opts=../../GaudiUPI/options/Errlog.opts -msgsvc=%msg_svc%"

rem start "prod_0" %GAUDIONLINEROOT%\%CMTCONFIG%\Gaudi.exe GaudiOnline mep_producer -n=prod_0 -p=333 -s=500 -r=2
type %~dpnx0
