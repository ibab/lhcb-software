#/bin/csh
rm /dev/shm/bm_*
rm /dev/shm/sem.TAN*
rm /dev/shm/TAN*
set EXEC=$ONLINEKERNELROOT/$CMTCONFIG/test.exe
set GEXEC=$GAUDIONLINEROOT/$CMTCONFIG/Gaudi.exe
set MINITERM='xterm -132 -im -sb -sl 500 -geometry 80x6   -e '
set WIDETERM='xterm -132 -im -sb -sl 500 -geometry 132x15 -e '
setenv PROCESSNAME MBM_0
xterm  -ls -132 -geometry 132x40 -e "$EXEC mep_install -s=10000 -e=30 -u=10 -i=RAW -c -i=EVENT -s=10000 -e=100 -u=10 -m" &
setenv PROCESSNAME ALLOC_0
$MINITERM "$EXEC tan_nameserver -a" &
sleep 3
setenv PROCESSNAME INQUIRE_0
$MINITERM "$EXEC tan_nameserver -i" &
setenv PROCESSNAME TANMON_0
$MINITERM "$EXEC tanmon -c" &
sleep 5
setenv PROCESSNAME NetRead_0
$WIDETERM "$GEXEC libMDF.so GaudiTask ../options/Network2MBM.opts" &
setenv PROCESSNAME NetRead_1
$WIDETERM "$GEXEC libMDF.so GaudiTask ../options/Network2MBM.opts" &
setenv PROCESSNAME NetRead_2
$WIDETERM "$GEXEC libMDF.so GaudiTask ../options/Network2MBM.opts" &
setenv PROCESSNAME NetWrite_0
$WIDETERM "$GEXEC libMDF.so GaudiTask ../options/MBM2Network.opts" &
setenv PROCESSNAME NetWrite_1
$WIDETERM "$GEXEC libMDF.so GaudiTask ../options/MBM2Network.opts" &
#
# test.exe amsc_sender -n=SEND_0 -s=pclhcb102::NetRead_0 -fac=25 -t=10 -len=1000
# test.exe mbm_prod -n=Prod_0 -s=45000 -b=RAW -m=100
