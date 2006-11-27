set DIM_DNS_NODE=%COMPUTERNAME%.cern.ch
set OPTS=%GAUDIONLINEROOT%/options
set gaudi_exe=%GAUDIONLINEROOT%\%CMTCONFIG%\Gaudi.exe GaudiOnline.dll OnlineTask -auto
set msg_svc=MessageSvc

set UTGID=MBMMon
start "%UTGID%" %GAUDIONLINEROOT%\%CMTCONFIG%\Gaudi.exe OnlineKernel.dll mbm_mon
set UTGID=MEPInit
start "%UTGID%" %gaudi_exe% -main=%OPTS%/MEPinit.opts -opt=%OPTS%/Daemon.opts -msgsvc=%msg_svc%
sleep 4
set UTGID=EvtProd
start "%UTGID%" %gaudi_exe% -opt=%OPTS%/MEPConverter.opts -msgsvc=%msg_svc%
set UTGID=EvtHolder
start "%UTGID%" %gaudi_exe% -opt=%OPTS%/MEPHolder.opts    -msgsvc=%msg_svc%
set UTGID=Moore_0
start "%UTGID%" %gaudi_exe% -opt=%OPTS%/ReadMBM.opts  -msgsvc=%msg_svc%
set UTGID=Moore_1
start "%UTGID%" %gaudi_exe% -opt=%OPTS%/ReadMBM.opts  -msgsvc=%msg_svc%
set UTGID=Moore_2
start "%UTGID%" %gaudi_exe% -opt=%OPTS%/ReadMBM.opts  -msgsvc=%msg_svc%
set UTGID=Moore_3
start "%UTGID%" %gaudi_exe% -opt=%OPTS%/ReadMBM.opts  -msgsvc=%msg_svc%
set UTGID=Moore_4
start "%UTGID%" %gaudi_exe% -opt=%OPTS%/ReadMBM.opts  -msgsvc=%msg_svc%
set UTGID=Moore_5
start "%UTGID%" %gaudi_exe% -opt=%OPTS%/ReadMBM.opts  -msgsvc=%msg_svc%
rem
rem
rem start "prod_0" %GAUDIONLINEROOT%\%CMTCONFIG%\Gaudi.exe GaudiOnline mep_producer -n=prod_0 -p=333 -s=500 -r=2
type %~d0%~p0\monitoring.bat
