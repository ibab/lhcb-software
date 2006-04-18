set test_exe=%ONLINEKERNELROOT%\win32_vc71_dbg\test.exe
set gaudi_exe=%GAUDIONLINEROOT%\win32_vc71_dbg\Gaudi.exe GaudiOnline.dll OnlineTask 
set UTGID=MBMMon_0  && start "Monitor" %test_exe% mbm_mon

set UTGID=MBMInit_0 && start %gaudi_exe% -options=..\options\MBMinit.opts -optopt=..\options\Daemon.opts -a
sleep 2
set UTGID=EvtProd_0 && start %gaudi_exe% -optopt=..\options\MEPConverter.opts -a
set UTGID=Holder_0  && start %gaudi_exe% -optopt=..\options\MEPHolder.opts    -a
set UTGID=Moore_0   && start %gaudi_exe% -optopt=..\options\ReadMBM.opts
set UTGID=Moore_1   && start %gaudi_exe% -optopt=..\options\ReadMBM.opts

rem start "prod_0" ..\win32_vc71_dbg\Gaudi.exe GaudiOnline mep_producer -n=prod_0 -p=333 -s=500 -r=2
