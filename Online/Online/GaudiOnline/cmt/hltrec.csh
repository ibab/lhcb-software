#/bin/csh
killall test.exe
killall Gaudi.exe
rm /dev/shm/bm_*
rm /dev/shm/sem.bm_*
rm /dev/shm/TAN*
rm /dev/shm/sem.TAN*
setenv TAN_PORT YES
set EXEC=$ONLINEKERNELROOT/$CMTCONFIG/test.exe
set GEXEC=$GAUDIONLINEROOT/$CMTCONFIG/Gaudi.exe
alias MINITERM 'xterm -132 -im -sb -sl 500 -geometry 80x6   -title $UTGID@$NODE -e '
alias WIDETERM 'xterm -132 -im -sb -sl 500 -geometry 132x20 -title $UTGID@$NODE -e '
alias WWIDETERM 'xterm -132 -im -sb -sl 500 -geometry 132x30 -title $UTGID@$NODE -e '
setenv UTGID MBM_0;   WWIDETERM "$EXEC mep_install -s=10000 -e=10 -u=5 -i=MEP -c -s=200 -e=200 -u=20 -i=EVENT -c -s=200 -e=200 -u=20 -i=RESULT -m" &
setenv UTGID ALLOC_0; MINITERM "$EXEC tan_nameserver -a -tcp" &
sleep 3
setenv UTGID TANMON_0;  MINITERM "$EXEC tanmon -c" &
setenv UTGID NetRead_0; WIDETERM "$GEXEC libMDF.so GaudiTask ../options/Network2MBM.opts" &
# setenv UTGID NetRead_1; WIDETERM "$GEXEC libMDF.so GaudiTask ../options/Network2MBM.opts" &
# setenv UTGID NetRead_2; WIDETERM "$GEXEC libMDF.so GaudiTask ../options/Network2MBM.opts" &
