set TAN_PORT=YES
set test_exe=%ONLINEKERNELROOT%\win32_vc71_dbg\test.exe
set gaudi_exe=%GAUDIONLINEROOT%\win32_vc71_dbg\Gaudi.exe GaudiOnline.dll OnlineTask 
set UTGID=MBMMon_0  && start "Monitor" %test_exe% mbm_mon
set UTGID=MBMInit_0 && start %gaudi_exe% -main=..\options\MBMinit.opts  -opt=..\options\Daemon.opts -msgsvc=LHCb::DimMessageSvc -a
set UTGID=TAN_0     && start %gaudi_exe% -main=..\options\TanAlloc.opts -opt=..\options\Daemon.opts -msgsvc=LHCb::DimMessageSvc -a
set UTGID=TANMON_0  && start "Tanmon"  %test_exe% tanmon -c
sleep 8
set UTGID=EvtProd_0 && start %gaudi_exe% -opt=..\options\MEPConverter.opts -msgsvc=LHCb::DimMessageSvc -a
set UTGID=Holder_0  && start %gaudi_exe% -opt=..\options\MEPHolder.opts    -msgsvc=LHCb::DimMessageSvc -a
set UTGID=Moore_0   && start %gaudi_exe% -opt=..\options\ReadMBM.opts      -msgsvc=LHCb::DimMessageSvc -a
set UTGID=Moore_1   && start %gaudi_exe% -opt=..\options\ReadMBM.opts      -msgsvc=LHCb::DimMessageSvc -a
rem set UTGID=Send_0    && start %gaudi_exe% -opt=..\options\FragmentSender.opts  -msgsvc=LHCb::DimMessageSvc -a
rem set UTGID=Send_1    && start %gaudi_exe% -opt=..\options\FragmentSender.opts  -msgsvc=LHCb::DimMessageSvc -a

rem start "prod_0" ..\win32_vc71_dbg\Gaudi.exe GaudiOnline mep_producer -n=prod_0 -p=333 -s=500 -r=2
