set TAN_PORT=YES
set TAN_NODE=%COMPUTERNAME%.cern.ch
set DIM_DNS_NODE=%COMPUTERNAME%.cern.ch
rem
set OPTS=%GAUDIONLINEROOT%/options
set gaudi_exe=%GAUDIONLINEROOT%\%CMTCONFIG%\Gaudi.exe GaudiOnline.dll OnlineTask -auto
set msg_svc=MessageSvc
set msg_svc=LHCb::DimMessageSvc
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
set UTGID=Moore_1
start "%UTGID%" %gaudi_exe% -opt=%OPTS%/ReadMBM.opts      -msgsvc=%msg_svc%
set UTGID=Moore_2
start "%UTGID%" %gaudi_exe% -opt=%OPTS%/ReadMBM.opts      -msgsvc=%msg_svc%
set UTGID=Spy_0
start "%UTGID%" %gaudi_exe% -opt=%OPTS%/SpyMBMSlow.opts   -msgsvc=%msg_svc%
set UTGID=Spy_1
start "%UTGID%" %gaudi_exe% -opt=%OPTS%/SpyMBM.opts       -msgsvc=%msg_svc%
set UTGID=Spy_2
start "%UTGID%" %gaudi_exe% -opt=%OPTS%/SpyMBMFast.opts   -msgsvc=%msg_svc%
rem
set UTGID=OutputBuffer
start "%UTGID%" %gaudi_exe% -main=%OPTS%/MBMinit.opts -opt=%OPTS%/Daemon.opts -msgsvc=%msg_svc%
set UTGID=Sender
start "%UTGID%" %gaudi_exe% -opt=%OPTS%/MDFSender.opts    -msgsvc=%msg_svc%
set UTGID=Receiver
start "%UTGID%" %gaudi_exe% -opt=%OPTS%/MDFReceiver.opts  -msgsvc=%msg_svc%
set UTGID=DiskWR
start "%UTGID%" %gaudi_exe% -opt=%OPTS%/DiskWR.opts       -msgsvc=%msg_svc%
set UTGID=MDFWriter
rem start "%UTGID%" %gaudi_exe% -opt=%OPTS%/MDFWriter.opts    -msgsvc=%msg_svc%

rem start /b xterm -132 -geometry 132x45  -title ErrorLogger  -e /bin/bash -l -c "export UTGID=ErrorLogger; cd J:/Gaudi/Online/UPI/cmt; ./upi.sh ../../GaudiOnline/%CMTCONFIG%/gaudi.exe ../../GaudiOnline/%CMTCONFIG%/GaudiOnline OnlineTask -opts=../../GaudiUPI/options/Errlog.opts -msgsvc=%msg_svc%"

rem start "prod_0" %GAUDIONLINEROOT%\%CMTCONFIG%\Gaudi.exe GaudiOnline mep_producer -n=prod_0 -p=333 -s=500 -r=2
type %~d0%~p0\hlt.bat
