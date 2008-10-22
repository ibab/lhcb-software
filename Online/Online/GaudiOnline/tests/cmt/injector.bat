@echo on
call %GAUDIONLINEROOT%\tests\cmt\preamble.bat
set NODENAME=%COMPUTERNAME%
if not defined PYOPTS set PYOPTS=%GAUDIONLINEROOT%\python& set PYTHONPATH=%GAUDIONLINEROOT%\python;%PYTHONPATH%

set UTGID=MBMMon
start "%UTGID%" %GAUDIONLINEROOT%\%CMTCONFIG%\Gaudi.exe OnlineKernel.dll mbm_mon
set UTGID=MBM
start "%UTGID%" %gaudi_exe% -main=%OPTS%/Main.opts -opt=command="import GaudiOnlineTests as g;g.runBuffer('Events')" -msgsvc=%msg_svc%
sleep 10
set UTGID=RDR_0
start "%UTGID%" %gaudi_exe% -main=%OPTS%/Main.opts -opt=command="import GaudiOnlineTests as g;g.runDimReader('Events')" -msgsvc=%msg_svc%
rem
rem
echo $BIGTERM MBMDump@${HOST} -e "export UTGID=${NODENAME}/MBMDump; $gaudi_run libMBMDump.so mbmdump" &
echo export UTGID=${NODENAME}/prod_0; $gaudi_run libGaudiOnline.so mep_producer -n=prod_0 -p=333 -s=500 -r=2

