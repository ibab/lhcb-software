rem
start %ONLINEKERNELROOT%\%CMTCONFIG\test.exe mbm_install -s=8096 -e=64 -u=64 -i=0
sleep 1
start %ONLINEKERNELROOT%\%CMTCONFIG\test.exe mbm_install -s=8096 -e=64 -u=64 -i=raw -m
sleep 3
start %ONLINEKERNELROOT%\%CMTCONFIG\test.exe mbm_cons      -b=0   -n=cons_0  -u -s=1
start %ONLINEKERNELROOT%\%CMTCONFIG\test.exe mbm_cons      -b=0   -n=cons_1  -u -s=5
start %ONLINEKERNELROOT%\%CMTCONFIG\test.exe mbm_cons      -b=0   -n=cons_2
start %ONLINEKERNELROOT%\%CMTCONFIG\test.exe mbm_cons -a   -b=raw -n=cons_a_0
start %ONLINEKERNELROOT%\%CMTCONFIG\test.exe mbm_cons -a   -b=raw -n=cons_a_1
start %ONLINEKERNELROOT%\%CMTCONFIG\test.exe mbm_cons -a   -b=raw -n=cons_a_2
start %ONLINEKERNELROOT%\%CMTCONFIG\test.exe mbm_cons -one -b=raw -n=one_0
start %ONLINEKERNELROOT%\%CMTCONFIG\test.exe mbm_cons -one -b=raw -n=one_1
start %ONLINEKERNELROOT%\%CMTCONFIG\test.exe mbm_cons -one -b=raw -n=one_2
sleep 2
start %ONLINEKERNELROOT%\%CMTCONFIG\test.exe mbm_prod     -b=0   -n=prod_0    -m=100000  -s=3500
rem start %ONLINEKERNELROOT%\%CMTCONFIG\test.exe mbm_prod     -b=raw -n=prod_1    -m=100000  -s=3500
