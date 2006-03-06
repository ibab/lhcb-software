#/bin/csh
rm /dev/shm/bm_*
rm /dev/shm/sem.bm_*
rm /dev/shm/TAN*
rm /dev/shm/sem.TAN*
set EXEC=$ONLINEKERNELROOT/$CMTCONFIG/test.exe
set GEXEC=$GAUDIONLINEROOT/$CMTCONFIG/Gaudi.exe
alias MINITERM 'xterm -132 -im -sb -sl 500 -geometry 80x6   -title $PROCESSNAME@$NODE -e '
alias WWIDETERM 'xterm -132 -im -sb -sl 500 -geometry 132x20 -title $PROCESSNAME@$NODE -e '
alias WIDETERM 'echo "Starting  $PROCESSNAME@$NODE"; '
#
setenv PROCESSNAME MBM_0;     WWIDETERM "$EXEC mep_install -s=10000 -e=30 -u=11 -i=EVENT -m" &
setenv PROCESSNAME ALLOC_0;   MINITERM "$EXEC tan_nameserver -a -tcp" &
sleep 2
setenv PROCESSNAME INQUIRE_0; MINITERM "$EXEC tan_nameserver -i -tcp" &
setenv PROCESSNAME TANMON_0;  MINITERM "$EXEC tanmon -c" &
sleep 8
setenv PROCESSNAME NetRead_0; WIDETERM $GEXEC libMDF.so GaudiTask ../options/Network2MBM.opts &
setenv PROCESSNAME NetRead_1; WIDETERM $GEXEC libMDF.so GaudiTask ../options/Network2MBM.opts &
setenv PROCESSNAME NetRead_2; WIDETERM $GEXEC libMDF.so GaudiTask ../options/Network2MBM.opts &
#setenv PROCESSNAME NetRead_3; WIDETERM $GEXEC libMDF.so GaudiTask ../options/Network2MBM.opts &
#setenv PROCESSNAME NetRead_4; WIDETERM $GEXEC libMDF.so GaudiTask ../options/Network2MBM.opts &
#setenv PROCESSNAME NetRead_5; WIDETERM "$GEXEC libMDF.so GaudiTask ../options/Network2MBM.opts" &
#setenv PROCESSNAME NetRead_6; WIDETERM "$GEXEC libMDF.so GaudiTask ../options/Network2MBM.opts" &
#setenv PROCESSNAME NetRead_7; WIDETERM "$GEXEC libMDF.so GaudiTask ../options/Network2MBM.opts" &
