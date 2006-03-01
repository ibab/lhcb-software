start "Install" %ONLINEKERNELROOT%\win32_vc71_dbg\test.exe mbm_install -s=8096 -e=64 -u=64 -i=EVENT -m
sleep 1
start "Cons_0" %ONLINEKERNELROOT%\win32_vc71_dbg\test.exe mbm_cons  -b=EVENT   -n=cons_0 -q
start "Cons_1" %ONLINEKERNELROOT%\win32_vc71_dbg\test.exe mbm_cons  -b=EVENT   -n=cons_1 -q
start "Cons_2" %ONLINEKERNELROOT%\win32_vc71_dbg\test.exe mbm_cons  -b=EVENT   -n=cons_2 -q
start "ReadDummyMDF" ..\win32_vc71_dbg\Gaudi.exe gaudi_task ..\options\ReadDummyMDF.opts
