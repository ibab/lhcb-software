start ..\win32_vc71_dbg\test.exe mbm_install -s=8096 -e=64 -u=64 -i=0
sleep 1
start ..\win32_vc71_dbg\test.exe mbm_install -s=8096 -e=64 -u=64 -i=raw -m
sleep 3
start ..\win32_vc71_dbg\test.exe mbm_cons     -b=0   -n=cons_0
start ..\win32_vc71_dbg\test.exe mbm_cons     -b=0   -n=cons_1
start ..\win32_vc71_dbg\test.exe mbm_cons     -b=0   -n=cons_2
start ..\win32_vc71_dbg\test.exe mbm_cons_a   -b=raw -n=cons_a_0
start ..\win32_vc71_dbg\test.exe mbm_cons_a   -b=raw -n=cons_a_1
start ..\win32_vc71_dbg\test.exe mbm_cons_a   -b=raw -n=cons_a_2
start ..\win32_vc71_dbg\test.exe mbm_cons_one -b=raw -n=one_0
start ..\win32_vc71_dbg\test.exe mbm_cons_one -b=raw -n=one_1
start ..\win32_vc71_dbg\test.exe mbm_cons_one -b=raw -n=one_2
sleep 2
start ..\win32_vc71_dbg\test.exe mbm_prod     -b=0   -n=prod_0    -m=100000  -s=3500
start ..\win32_vc71_dbg\test.exe mbm_prod     -b=raw -n=prod_1    -m=100000  -s=3500
