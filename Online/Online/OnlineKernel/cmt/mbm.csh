rm shm/*
xterm -ls -132 -e "../slc3_ia32_gcc323_dbg/test.exe mbm_install -s=8096 -e=64 -u=64 -m" &
sleep 1
xterm -e "../slc3_ia32_gcc323_dbg/test.exe mbm_cons     -n=cons_0" &
xterm -e "../slc3_ia32_gcc323_dbg/test.exe mbm_cons     -n=cons_1" &
xterm -e "../slc3_ia32_gcc323_dbg/test.exe mbm_cons     -n=cons_2" &
#xterm -e "../slc3_ia32_gcc323_dbg/test.exe mbm_cons_a   -n=cons_a_0" &
#xterm -e "../slc3_ia32_gcc323_dbg/test.exe mbm_cons_a   -n=cons_a_1" &
#xterm -e "../slc3_ia32_gcc323_dbg/test.exe mbm_cons_a   -n=cons_a_2" &
xterm -e "../slc3_ia32_gcc323_dbg/test.exe mbm_cons_one -n=one_0" &
xterm -e "../slc3_ia32_gcc323_dbg/test.exe mbm_cons_one -n=one_1" &
xterm -e "../slc3_ia32_gcc323_dbg/test.exe mbm_cons_one -n=one_2" &
sleep 3
echo xterm -e "../slc3_ia32_gcc323_dbg/test.exe mbm_prod     -n=prod_0    -m=10  -s=3500" &
