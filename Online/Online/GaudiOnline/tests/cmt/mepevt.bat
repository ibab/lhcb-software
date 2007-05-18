set TAN_PORT=YES
set TAN_NODE=%COMPUTERNAME%.cern.ch
set test_exe=%ONLINEKERNELROOT%\win32_vc71_dbg\test.exe
set gaudi_exe=%GAUDIONLINEROOT%\win32_vc71_dbg\Gaudi.exe GaudiOnline.dll OnlineTask 
set msg_svc=MessageSvc
set msg_svc=LHCb::DimMessageSvc
set UTGID=MBMMon
start "%UTGID%" %test_exe% mbm_mon
set UTGID=MBMInit
start "%UTGID%" %gaudi_exe% -main=../options/MEPinit.opts -opt=../options/Daemon.opts -msgsvc=%msg_svc% -auto
set UTGID=ErrSrv
start "%UTGID%" %gaudi_exe% -opt=../options/ErrorSrv.opts     -msgsvc=%msg_svc%  -auto
set UTGID=ErrLog
start "%UTGID%" %gaudi_exe% -opt=../options/ErrorLogger.opts  -msgsvc=MessageSvc -auto
sleep 3
set UTGID=EvtProd
start "%UTGID%" %gaudi_exe% -opt=../options/MEPConverter.opts -msgsvc=%msg_svc%  -auto
set UTGID=EvtHolder
start "%UTGID%" %gaudi_exe% -opt=../options/MEPHolder.opts    -msgsvc=%msg_svc%  -auto
set UTGID=MEPcons
start "%UTGID%" %gaudi_exe% -opt=../options/MEPAnalyser.opts  -msgsvc=%msg_svc%  -auto

rem start /b xterm -132 -geometry 132x45  -title ErrorLogger  -e /bin/bash -l -c "export UTGID=ErrorLogger; cd J:/Gaudi/Online/UPI/cmt; ./upi.sh ../../GaudiOnline/win32_vc71_dbg/gaudi.exe GaudiOnline OnlineTask -opts=../../GaudiUPI/options/Errlog.opts -msgsvc=%msg_svc% -auto"

rem start "prod_0" ..\win32_vc71_dbg\Gaudi.exe GaudiOnline mep_producer -n=prod_0 -p=333 -s=500 -r=2
