set TAN_PORT=YES
set TAN_NODE=%COMPUTERNAME%.cern.ch
set test_exe=%ONLINEKERNELROOT%\win32_vc71_dbg\test.exe
set gaudi_exe=%GAUDIONLINEROOT%\win32_vc71_dbg\Gaudi.exe GaudiOnline.dll OnlineTask 
rem set UTGID=MBMMon    && start "Monitor" %test_exe% mbm_mon
set UTGID=MBMInit   && start %gaudi_exe% -main=..\options\MBMinit.opts  -opt=..\options\Daemon.opts -msgsvc=LHCb::DimMessageSvc -a
rem set UTGID=TANServ   && start %gaudi_exe% -main=..\options\TanAlloc.opts -opt=..\options\Daemon.opts -msgsvc=LHCb::DimMessageSvc -a
rem set UTGID=TANMon    && start "Tanmon"  %test_exe% tanmon -c
set UTGID=ErrServ   && start %gaudi_exe% -opt=..\options\ErrorSrv.opts -msgsvc=LHCb::DimMessageSvc -a
sleep 8
set UTGID=EvtProd   && start %gaudi_exe% -opt=..\options\MEPConverter.opts -msgsvc=LHCb::DimMessageSvc -a
set UTGID=EvtHolder && start %gaudi_exe% -opt=..\options\MEPHolder.opts    -msgsvc=LHCb::DimMessageSvc -a
set UTGID=Moore_0   && start %gaudi_exe% -opt=..\options\ReadMBM.opts      -msgsvc=LHCb::DimMessageSvc -a
set UTGID=Moore_1   && start %gaudi_exe% -opt=..\options\ReadMBM.opts      -msgsvc=LHCb::DimMessageSvc -a
set UTGID=Sender    && start %gaudi_exe% -opt=..\options\FragmentSender.opts  -msgsvc=LHCb::DimMessageSvc -a -debug
rem
rem start "prod_0" ..\win32_vc71_dbg\Gaudi.exe GaudiOnline mep_producer -n=prod_0 -p=333 -s=500 -r=2
