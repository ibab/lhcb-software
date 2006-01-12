#/bin/csh
rm /dev/shm/bm_*
set EXEC=$ONLINEKERNELROOT/$CMTCONFIG/test.exe
set GEXEC=$GAUDIONLINEROOT/$CMTCONFIG/Gaudi.exe
set MINITERM='xterm -geometry 60x6 -e '
$MINITERM "$EXEC mbm_install -s=10000 -e=5 -u=6 -i=MEP" &
sleep 1
$MINITERM "$EXEC mbm_install -s=4096 -e=50 -u=10 -i=EVENT" &
sleep 1
$MINITERM "$EXEC mbm_install -s=4096 -e=50 -u=5 -i=RESULT" &
sleep 1
xterm  -ls -132 -geometry 132x45 -e "$EXEC mbm_mon" &
sleep 1
$MINITERM "$EXEC  mep_holder_a -i=MEP    -n=MepHold_0 -p=333" &
$MINITERM "$GEXEC libGaudiOnline.so mep2event_prod -n=evtprod_0 -p=333" &
xterm  -ls -132 -geometry 132x45 -e "$GEXEC libMDF.so GaudiTask ../options/ReadMBM.opts" &
echo $MINITERM "$GEXEC libGaudiOnline.so mep_producer -n=prod_0 -p=333 -s=500" &

