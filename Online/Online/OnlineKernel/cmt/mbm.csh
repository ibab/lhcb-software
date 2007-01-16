#/bin/csh
rm /dev/shm/bm_*
rm /dev/shm/sem.BM*
rm /dev/shm/sem.bm*
rm /dev/shm/sem.MEP*
xterm -ls -132 -e "${ONLINEKERNELROOT}/${CMTCONFIG}/test.exe mbm_install -s=8096 -e=64 -u=64 -i=0" &
sleep 1
xterm -ls -132 -geometry 132x45 -e "${ONLINEKERNELROOT}/${CMTCONFIG}/test.exe mbm_mon" &
xterm -e "${ONLINEKERNELROOT}/${CMTCONFIG}/test.exe mbm_cons     -n=cons_s_0" &
xterm -e "${ONLINEKERNELROOT}/${CMTCONFIG}/test.exe mbm_cons     -n=cons_s_1" &
xterm -e "${ONLINEKERNELROOT}/${CMTCONFIG}/test.exe mbm_cons     -n=cons_s_2" &
xterm -e "${ONLINEKERNELROOT}/${CMTCONFIG}/test.exe mbm_cons_a   -n=cons_a_0" &
xterm -e "${ONLINEKERNELROOT}/${CMTCONFIG}/test.exe mbm_cons_a   -n=cons_a_1" &
xterm -e "${ONLINEKERNELROOT}/${CMTCONFIG}/test.exe mbm_cons_a   -n=cons_a_2" &
xterm -e "${ONLINEKERNELROOT}/${CMTCONFIG}/test.exe mbm_cons_one -n=cons_o_0" &
xterm -e "${ONLINEKERNELROOT}/${CMTCONFIG}/test.exe mbm_cons_one -n=cons_o_1" &
xterm -e "${ONLINEKERNELROOT}/${CMTCONFIG}/test.exe mbm_cons_one -n=cons_o_2" &
echo "${ONLINEKERNELROOT}/${CMTCONFIG}/test.exe mbm_prod     -n=prod_0    -m=1000  -s=3500"
