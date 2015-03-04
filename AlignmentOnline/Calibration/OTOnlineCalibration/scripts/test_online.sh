export LOGFIFO=/tmp/logOTCalib.fifo;
export PARTITION="FEST";
export PARTITION_NAME="FEST";
HOSTUP=`echo $HOSTNAME | tr a-z A-Z`
export UTGID=${PARTITION}_${HOSTUP}_OTCALIB;
export TNS_ADMIN=/sw/oracle/10.2.0.4/linux64/network/admin;
export DIM_DNS_NODE=mona09;

sh -c "exec -a ${UTGID}_LogSrv /opt/FMC/sbin64/logSrv -N ${DIM_DNS_NODE} -v -s fest -l 1 -S 1 -p ${LOGFIFO}" &
xterm -ls -132 -geometry 170x50 -title LogViewer -bg black -e "LOGFIFO=${LOGFIFO} exec -a ${UTGID}_LogViewer /opt/FMC/bin/logViewer -sl 20000 -N ${DIM_DNS_NODE} -m ${HOSTNAME} -l 2 -S -s fest" &

exec -a ${UTGID} GaudiOnlineExe.exe libGaudiOnline.so OnlineTask -auto \
    -msgsvc=LHCb::FmcMessageSvc \
    -tasktype=LHCb::Class1Task \
    -main=Main.opts \
    -opt=command="\
import os; from Gaudi.Configuration import importOptions;\
importOptions(os.environ['OTONLINECALIBRATIONROOT']+'/scripts/OTOnlineCalib.py');"

