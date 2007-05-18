set TAN_PORT=YES
set TAN_NODE=%COMPUTERNAME%.cern.ch
set DIM_DNS_NODE=%COMPUTERNAME%.cern.ch
rem
set test_exe=%ONLINEKERNELROOT%\win32_vc71_dbg\test.exe
set gaudi_exe=%GAUDIONLINEROOT%\win32_vc71_dbg\Gaudi.exe GaudiOnline.dll OnlineTask -auto
set msg_svc=MessageSvc
set msg_svc=LHCb::DimMessageSvc
set UTGID=DNS
start "%UTGID%" dns
set UTGID=MBMMon
start "%UTGID%" %test_exe% mbm_mon
set UTGID=ErrSrv
start "%UTGID%" %gaudi_exe% -opt=../options/ErrorSrv.opts     -msgsvc=%msg_svc%
set UTGID=ErrLog
start "%UTGID%" %gaudi_exe% -opt=../options/ErrorLogger.opts  -msgsvc=MessageSvc
sleep 3
set UTGID=MBMInit
start "%UTGID%" %gaudi_exe% -main=../options/MBMinit.opts -opt=../options/Daemon.opts -msgsvc=%msg_svc%
sleep 2
set UTGID=Prod
start "%UTGID%" %gaudi_exe% -opt=../options/MDF2MBM.opts -msgsvc=%msg_svc% -debug

