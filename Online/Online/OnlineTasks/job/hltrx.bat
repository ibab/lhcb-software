set TAN_PORT=YES
set TAN_NODE=%COMPUTERNAME%.cern.ch
set test_exe=%ONLINEKERNELROOT%\win32_vc71_dbg\test.exe
set gaudi_exe=%GAUDIONLINEROOT%\win32_vc71_dbg\Gaudi.exe GaudiOnline.dll OnlineTask 
set msg_svc=LHCb::DimMessageSvc
set msg_svc=MessageSvc
set UTGID=MBMMon
start "%UTGID%" %test_exe% mbm_mon
set UTGID=MBMInit
start "%UTGID%" %gaudi_exe% -main=../options/MBMinit.opts -opt=../options/Daemon.opts -msgsvc=%msg_svc% -auto
rem set UTGID=TANMon
rem start "%UTGID%"  %test_exe% tanmon -c
set UTGID=ErrServ
start "%UTGID%" %gaudi_exe% -opt=../options/ErrorSrv.opts     -msgsvc=%msg_svc% -auto
sleep 8
set UTGID=EvtProd
start "%UTGID%" %gaudi_exe% -opt=../options/MEPConverter.opts -msgsvc=%msg_svc% -auto
set UTGID=EvtHolder
start "%UTGID%" %gaudi_exe% -opt=../options/MEPHolder.opts    -msgsvc=%msg_svc% -auto
set UTGID=Moore_0
start "%UTGID%" %gaudi_exe% -opt=../options/ReadMBM.opts      -msgsvc=%msg_svc% -auto
set UTGID=Moore_1
start "%UTGID%" %gaudi_exe% -opt=../options/ReadMBM.opts      -msgsvc=%msg_svc% -auto
set UTGID=ErrorLogger
start "%UTGID%" %gaudi_exe% -opt=../options/ErrorLogger.opts  -msgsvc=MessageSvc -auto
set UTGID=MEPRx
start "%UTGID%" %gaudi_exe% -opt=../options/MEPRxSvc.opts     -msgsvc=MessageSvc -auto
