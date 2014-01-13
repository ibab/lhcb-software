set test_exe=%GAUDIONLINEROOT%\%CMTCONFIG%\Gaudi.exe OnlineKernel
set gaudi_exe=%GAUDIONLINEROOT%\%CMTCONFIG%\Gaudi.exe
set OPTS=%GAUDIONLINEROOT%\options
start "Install"       %test_exe% mep_install -s=5000 -e=100 -u=20 -i=MEP -c -s=400 -e=400 -u=14 -i=EVENT -c -s=400 -e=400 -u=14 -i=RESULT
sleep 1
start "Monitor"       %test_exe% mbm_mon
start "Bitmaps"       %test_exe% mep_dump_bitmap -b1=MEP -b2=EVENT -b3=RESULT -c
start "mephold_0"     %test_exe% mep_holder_a -i=MEP -n=mephold_0 -p=333
start "evtprod_0"     %gaudi_exe% GaudiOnline mep2event_prod -n=evtprod_0 -p=333 -r=1
start "diskwr_0"      %test_exe% mep_cons_a -i=RESULT -n=diskwr_0 -p=333
start "diskwr_1"      %test_exe% mep_cons_a -i=RESULT -n=diskwr_1 -p=333
set PROCESSNAME=moore_0
start "Moore" %gaudi_exe% MDF GaudiTask .\ReadMBM.opts
set PROCESSNAME=moore_1
start "Moore" %gaudi_exe% MDF GaudiTask .\ReadMBM.opts
set PROCESSNAME=moore_2
start "Moore" %gaudi_exe% MDF GaudiTask .\ReadMBM.opts

rem start "prod_0" %gaudi_exe% GaudiOnline mep_producer -n=prod_0 -p=333 -s=500 -r=2
type %~dpnx0
