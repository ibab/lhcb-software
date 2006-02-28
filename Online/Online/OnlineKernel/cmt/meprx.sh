#!/bin/bash

sudo rm -f /dev/shm/bm_*
sudo rm -f /dev/shm/sem.BM*
sudo rm -f /dev/shm/sem.bm*
sudo rm -f /dev/shm/sem.MEP*
sudo killall test.exe; killall Gaudi.exe; 
EXEC=../$CMTDEB/test.exe
GEXEC=$GAUDIONLINEROOT/$CMTCONFIG/Gaudi.exe
MINITERM='xterm -geometry 60x6 -e '
ICONTERM='xterm  -e '
GTERM='gnome-terminal -e '
$ICONTERM "$EXEC mep_install -s=231072 -e=5  -u=4 -i=MEP -c -s=4096 -e=30 -u=10 -i=EVENT -c -s=4096 -e=30 -u=10 -i=RESULT" &
#$MINITERM "$EXEC mep_install -s=10000 -e=10 -u=5 -i=MEP -c -s=200 -e=200 -u=20 -i=EVENT -c -s=200 -e=200 -u=20 -i=RESULT" &

#$sleep 1
#$$ICONTERM "$EXEC mbm_install -s=4096 -e=30 -u=10 -i=EVENT" &
#sleep 1
#$ICONTERM "$EXEC mbm_install -s=4096 -e=30 -u=10 -i=RESULT" &
sleep 1
xterm  -ls -132 -geometry 132x45 -e "$EXEC mbm_mon" &
sleep 1
$MINITERM "$EXEC mep_reform_a -i=EVENT  -o=RESULT -n=Moore_0" &
$MINITERM "$EXEC mep_reform_a -i=EVENT  -o=RESULT -n=Moore_1" &
$MINITERM "$EXEC mep_reform_a -i=EVENT  -o=RESULT -n=Moore_2" &
$MINITERM "$EXEC mep_reform_a -i=EVENT  -o=RESULT -n=Moore_3" &
#$MINITERM "$EXEC mep_reform_a -i=EVENT  -o=RESULT -n=Moore_4" &
#$MINITERM "$EXEC mep_cons_a   -i=RESULT -n=Diskwr_0" &
#$MINITERM "$EXEC mep_cons_a   -i=RESULT -n=LBMonit_0" &
$MINITERM "$EXEC mep_cons_a   -i=RESULT -n=LBMonit_1" &
case $1 in
    rx)
	echo "Starting meprx in debugger..."
	$MINITERM "$GEXEC libGaudiOnline.so mep2event_prod -n=evtprod_0 -r=1" &
	sudo gdb -x meprx.gdb ../$CMTCONFIG/test.exe
	;;
    pr) echo "Starting mepproducer in debugger..."
	xterm -e sudo ../$CMTCONFIG/test.exe meprx &
	gdb -x meprx.gdb $GEXEC
        ;;
    go)  echo "Starting all" 
	$MINITERM "$GEXEC libGaudiOnline.so mep2event_prod -n=evtprod_0 -r=1" &
	sleep 1 
	 xterm -title "meprx on $(hostname)" -e sudo ../$CMTCONFIG/test.exe meprx -maxevent=1&
	;;
    *)  ;;
esac
$MINITERM "$EXEC mep_holder_a -i=MEP    -n=MepHold_0" & 


