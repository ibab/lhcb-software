start %ONLINEKERNELROOT%\%CMTCONFIG\test.exe mbm_install -s=8096 -e=10 -u=20 -i=MEP
sleep 1
start %ONLINEKERNELROOT%\%CMTCONFIG\test.exe mbm_install -s=4096 -e=50 -u=20 -i=EVENT
sleep 1
start %ONLINEKERNELROOT%\%CMTCONFIG\test.exe mbm_install -s=4096 -e=100 -u=20 -i=RESULT
sleep 1
start %ONLINEKERNELROOT%\%CMTCONFIG\test.exe mbm_mon
sleep 1
start %ONLINEKERNELROOT%\%CMTCONFIG\test.exe mep_reform_a -i=EVENT  -o=RESULT -n=Moore_0
start %ONLINEKERNELROOT%\%CMTCONFIG\test.exe mep_reform_a -i=EVENT  -o=RESULT -n=Moore_1
start %ONLINEKERNELROOT%\%CMTCONFIG\test.exe mep_reform_a -i=EVENT  -o=RESULT -n=Moore_2
start %ONLINEKERNELROOT%\%CMTCONFIG\test.exe mep_reform_a -i=EVENT  -o=RESULT -n=Moore_3
start %ONLINEKERNELROOT%\%CMTCONFIG\test.exe mep_reform_a -i=EVENT  -o=RESULT -n=Moore_4
start %ONLINEKERNELROOT%\%CMTCONFIG\test.exe mep_cons_a   -i=RESULT -n=Diskwr_0
start %ONLINEKERNELROOT%\%CMTCONFIG\test.exe mep_cons_a   -i=RESULT -n=LBMonit_0
start %ONLINEKERNELROOT%\%CMTCONFIG\test.exe mep_holder_a -i=MEP    -n=MepHold_0
sleep 2
start %ONLINEKERNELROOT%\%CMTCONFIG\test.exe mep_prod_a -n=Mep_0
