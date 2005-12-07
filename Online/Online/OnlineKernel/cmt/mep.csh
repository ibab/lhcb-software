#/bin/csh
rm /dev/shm/bm_*
rm /dev/shm/sem.BM*
rm /dev/shm/sem.bm*
rm /dev/shm/sem.MEP*
set EXEC=../slc3_ia32_gcc323/test.exe
set MINITERM='xterm -geometry 60x6 -e '
$MINITERM "$EXEC mbm_install -s=8096 -e=10 -u=20 -i=MEP" &
sleep 1
$MINITERM "$EXEC mbm_install -s=4096 -e=50 -u=20 -i=EVENT" &
sleep 1
$MINITERM "$EXEC mbm_install -s=4096 -e=100 -u=20 -i=RESULT" &
sleep 1
xterm  -ls -132 -geometry 132x45 -e "$EXEC mbm_mon" &
sleep 1
$MINITERM "$EXEC mep_reform_a -i=EVENT  -o=RESULT -n=Moore_0" &
$MINITERM "$EXEC mep_reform_a -i=EVENT  -o=RESULT -n=Moore_1" &
$MINITERM "$EXEC mep_reform_a -i=EVENT  -o=RESULT -n=Moore_2" &
$MINITERM "$EXEC mep_reform_a -i=EVENT  -o=RESULT -n=Moore_3" &
$MINITERM "$EXEC mep_reform_a -i=EVENT  -o=RESULT -n=Moore_4" &
$MINITERM "$EXEC mep_cons_a   -i=RESULT -n=Diskwr_0" &
#$MINITERM "$EXEC mep_cons_a   -i=RESULT -n=LBMonit_0" &
$MINITERM "$EXEC mep_holder_a -i=MEP    -n=MepHold_0" &
sleep 2
$MINITERM "$EXEC mep_prod_a -n=Mep_0" &
