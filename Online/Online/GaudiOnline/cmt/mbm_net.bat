set test_exe=%ONLINEKERNELROOT%\win32_vc71_dbg\test.exe
set gaudi_exe=%GAUDIONLINEROOT%\win32_vc71_dbg\Gaudi.exe
set PROCESSNAME=MBM_0
start "Install" %test_exe% mep_install  -s=10000 -e=30 -u=10 -i=RAW -c -i=EVENT -s=10000 -e=100 -u=10 -m
set PROCESSNAME=ALLOC_0
start "%PROCESSNAME%" %ONLINEKERNELROOT%\%CMTCONFIG%\test.exe tan_nameserver -a
set PROCESSNAME=INQUIRE_0
sleep 1
start "%PROCESSNAME%" %ONLINEKERNELROOT%\%CMTCONFIG%\test.exe tan_nameserver -i -tcp
set PROCESSNAME=TANMON_0
start "%PROCESSNAME%" %ONLINEKERNELROOT%\%CMTCONFIG%\test.exe tanmon -c
set PROCESSNAME=NetRead_0
start "%PROCESSNAME%" %gaudi_exe% MDF GaudiTask ..\options\Network2MBM.opts
set PROCESSNAME=NetRead_1
start "%PROCESSNAME%" %gaudi_exe% MDF GaudiTask ..\options\Network2MBM.opts
set PROCESSNAME=NetRead_2
start "%PROCESSNAME%" %gaudi_exe% MDF GaudiTask ..\options\Network2MBM.opts
set PROCESSNAME=NetWrite_0
start "%PROCESSNAME%" %gaudi_exe% MDF GaudiTask ..\options\MBM2Network.opts
set PROCESSNAME=NetWrite_1
start "%PROCESSNAME%" %gaudi_exe% MDF GaudiTask ..\options\MBM2Network.opts
rem
rem test.exe amsc_sender -n=SEND_0 -s=pclhcb102::NetRead_0 -fac=25 -t=10 -len=1000
rem test.exe mbm_prod -n=Prod_0 -s=45000 -b=RAW -m=100
