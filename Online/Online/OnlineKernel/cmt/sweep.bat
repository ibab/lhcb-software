start ..\win32_vc71_dbg\test.exe mbm_install -s=40002 -e=64 -u=64 -i=0
start ..\win32_vc71_dbg\test.exe mbm_install -s=200 -e=64 -u=64 -b=9 -i=Raw
start ..\win32_vc71_dbg\test.exe mbm_mon
start ..\win32_vc71_dbg\test.exe mep_dump_bitmap -b1=0 -b2=Raw -c
sleep 1
start ..\win32_vc71_dbg\test.exe mbm_cons     -b=0   -n=cons_0
start ..\win32_vc71_dbg\test.exe mbm_cons     -b=0   -n=cons_1
rem start ..\win32_vc71_dbg\test.exe mbm_prod     -b=0   -n=prod_0    -m=100000  -s=16 -l=16 -u=1000000
