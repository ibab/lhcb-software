#/bin/csh
rm /dev/shm/bm_*
killall test.exe
killall Gaudi.exe
set EXEC=$ONLINEKERNELROOT/$CMTCONFIG/test.exe
set GEXEC=$GAUDIONLINEROOT/$CMTCONFIG/Gaudi.exe
set MINITERM='xterm -geometry 60x6 -e '
$MINITERM "$EXEC mep_install -s=10000 -e=10 -u=5 -i=MEP -c -s=200 -e=200 -u=20 -i=EVENT -c -s=200 -e=200 -u=20 -i=RESULT" &
sleep 5
xterm  -ls -132 -geometry 132x35 -e "$EXEC mbm_mon" &
xterm  -ls -132 -geometry 132x25 -e "$EXEC mep_dump_bitmap -b1=MEP -b2=EVENT -b3=RESULT -c" &
sleep 1
$MINITERM "$EXEC  mep_holder_a -i=MEP    -n=MepHold_0 -p=333" &
$MINITERM "$GEXEC libGaudiOnline.so mep2event_prod -n=evtprod_0 -p=333 -r=1" &
xterm  -ls -132 -geometry 132x45 -e "setenv PROCESSNAME Moore_0; $GEXEC libMDF.so GaudiTask ReadMBM.opts" &
xterm  -ls -132 -geometry 132x45 -e "setenv PROCESSNAME Moore_1; $GEXEC libMDF.so GaudiTask ReadMBM.opts" &
xterm  -ls -132 -geometry 132x45 -e "setenv PROCESSNAME Moore_2; $GEXEC libMDF.so GaudiTask ReadMBM.opts" &
$MINITERM "$EXEC mep_cons_a -i=RESULT -n=Diskwr_0 -p=333" &
$MINITERM "$EXEC mep_cons_a -i=RESULT -n=Diskwr_1 -p=333" &
echo $MINITERM "$GEXEC libGaudiOnline.so mep_producer -n=prod_0 -p=333 -s=500 -r=2" &
cat mep.csh
