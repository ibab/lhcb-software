#/bin/csh
rm /dev/shm/bm_*
rm /dev/shm/sem.bm_*
rm /dev/shm/TAN*
rm /dev/shm/sem.TAN*
set EXEC=$ONLINEKERNELROOT/$CMTCONFIG/test.exe
set GEXEC=$GAUDIONLINEROOT/$CMTCONFIG/Gaudi.exe
alias MINITERM 'xterm -132 -im -sb -sl 500 -geometry 80x6   -title $PROCESSNAME@$NODE -e '
alias WIDETERM 'xterm -132 -im -sb -sl 500 -geometry 132x20 -title $PROCESSNAME@$NODE -e '
setenv PROCESSNAME MBM_0
WIDETERM "$EXEC mep_install -s=10000 -e=30 -u=10 -i=RAW -m" &
setenv PROCESSNAME ALLOC_0
MINITERM "$EXEC tan_nameserver -a -tcp" &
sleep 3
setenv PROCESSNAME INQUIRE_0
MINITERM "$EXEC tan_nameserver -i -tcp" &
setenv PROCESSNAME TANMON_0
MINITERM "$EXEC tanmon -c" &
sleep 2
#ddd $GEXEC &
#echo libMDF.so GaudiTask ../options/MBM2Network.opts
setenv PROCESSNAME NetWrite_0; WIDETERM "$GEXEC libMDF.so GaudiTask ../options/MBM2Network.opts" &
setenv PROCESSNAME NetWrite_1; WIDETERM "$GEXEC libMDF.so GaudiTask ../options/MBM2Network.opts" &
setenv PROCESSNAME NetWrite_2; WIDETERM "$GEXEC libMDF.so GaudiTask ../options/MBM2Network.opts" &
setenv PROCESSNAME NetWrite_3; WIDETERM "$GEXEC libMDF.so GaudiTask ../options/MBM2Network.opts" &
setenv PROCESSNAME NetWrite_4; WIDETERM "$GEXEC libMDF.so GaudiTask ../options/MBM2Network.opts" &
setenv PROCESSNAME NetWrite_5; WIDETERM "$GEXEC libMDF.so GaudiTask ../options/MBM2Network.opts" &
setenv PROCESSNAME NetWrite_6; WIDETERM "$GEXEC libMDF.so GaudiTask ../options/MBM2Network.opts" &
setenv PROCESSNAME NetWrite_7; WIDETERM "$GEXEC libMDF.so GaudiTask ../options/MBM2Network.opts" &
#
# test.exe amsc_sender -n=SEND_0 -s=pclhcb102::NetRead_0 -fac=25 -t=10 -len=1000
echo test.exe mbm_prod -n=Prod_0 -s=45000 -b=RAW -m=100
