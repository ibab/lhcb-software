rm shm/*
xterm -ls -132 -e "../slc3_ia32_gcc323_dbg/test.exe mbm_install -s=8096 -e=64 -u=64 -m" &
sleep 1
xterm -e "../slc3_ia32_gcc323_dbg/test.exe mbm_cons     -n=cons_s_0" &
xterm -e "../slc3_ia32_gcc323_dbg/test.exe mbm_cons     -n=cons_s_1" &
xterm -e "../slc3_ia32_gcc323_dbg/test.exe mbm_cons     -n=cons_s_2" &
xterm -e "../slc3_ia32_gcc323_dbg/test.exe mbm_cons_a   -n=cons_a_0" &
xterm -e "../slc3_ia32_gcc323_dbg/test.exe mbm_cons_a   -n=cons_a_1" &
xterm -e "../slc3_ia32_gcc323_dbg/test.exe mbm_cons_a   -n=cons_a_2" &
xterm -e "../slc3_ia32_gcc323_dbg/test.exe mbm_cons_one -n=cons_o_0" &
xterm -e "../slc3_ia32_gcc323_dbg/test.exe mbm_cons_one -n=cons_o_1" &
xterm -e "../slc3_ia32_gcc323_dbg/test.exe mbm_cons_one -n=cons_o_2" &
echo xterm -e "../slc3_ia32_gcc323_dbg/test.exe mbm_prod     -n=prod_0    -m=10  -s=3500" &
