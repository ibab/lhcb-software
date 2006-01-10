start %ONLINEKERNELROOT%\win32_vc71_dbg\test.exe mbm_install -s=8096 -e=64 -u=64 -i=EVENT -m
sleep 1
start %ONLINEKERNELROOT%\win32_vc71_dbg\test.exe mbm_cons  -b=EVENT   -n=cons_0 -q
start %ONLINEKERNELROOT%\win32_vc71_dbg\test.exe mbm_cons  -b=EVENT   -n=cons_1 -q
start %ONLINEKERNELROOT%\win32_vc71_dbg\test.exe mbm_cons  -b=EVENT   -n=cons_2 -q
start ..\win32_vc71_dbg\Gaudi.exe gaudi_task ..\options\ReadDummyMDF.opts
