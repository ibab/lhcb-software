#!/bin/bash
export PID=$$
export NEVT=1000000
export NEVT=500000
export LOG_FILE=GaudiExample.ROOTIO.${PID}.txt
export DISK=/opt/dirac/${USER}

echo `date` > ${LOG_FILE}

cd /afs/cern.ch/user/f/frankb/cmtuser
. `which setenvProject.sh` Gaudi v21r7
cd /afs/cern.ch/user/f/frankb/cmtuser/Gaudi_v21r7/Online/RootCnv/cmt
export CMTCONFIG=$CMTOPT
. setup.sh

run()
{
tcsh <<EOF
time  `which Gaudi.exe` ${1} > ${1}.log
EOF
}

flush_disk()
{
  if test "${1}" = "file"
  then
    echo "Flushing existing disk cache......";
    sudo /afs/cern.ch/user/f/frankb/cmtuser/Gaudi_v21r7/Online/RootCnv/cmt/flush_caches.sh;
  fi;
}

runtests()
{
export DATA_DIR=${1}:${2}
echo "Data directory: ${DATA_DIR}" >> ${LOG_FILE}
#
# =============================================================================================
#
cat > opt_${PID}.opts <<EOF
#include "$ROOTCNVROOT/options/Setup.opts"
ApplicationMgr.OutStream  += { "OutputStream/RootDst"};
ApplicationMgr.TopAlg      = { "WriteAlg", "ReadAlg" };
ApplicationMgr.TopAlg      = { "WriteAlg" };
ReadAlg.OutputLevel        = 4;
EventSelector.PrintFreq    = 1;
ApplicationMgr.EvtSel      = "NONE"; // do not use any event input
RootDst.ItemList           = { "/Event#999" } ;
RootDst.Output             = "DATAFILE='PFN:${DATA_DIR}/ROOTIO_${PID}.dst' SVC='RootCnvSvc' OPT='RECREATE'";
ApplicationMgr.EvtMax      = ${NEVT};
RootCnvSvc.CacheSize         = 50000000;
EOF
#
if test "${1}" = "castor"
then
  rfcp /opt/dirac/frankb/ROOTIO.dst /castor/cern.ch/user/f/frankb/data/ROOTIO_${PID}.dst;
else
  ln -s /opt/dirac/frankb/ROOTIO.dst /opt/dirac/frankb/ROOTIO_${PID}.dst;
fi;
#echo "Write ROOTIO to ${1} $NEVT events" >> ${LOG_FILE}
#flush_disk ${1}
#run opt_${PID}.opts >> ${LOG_FILE}
#cat opt_${PID}.opts.log | grep "RootDst:execute" >> ${LOG_FILE}
#echo " " >> ${LOG_FILE}
#
# =============================================================================================
#
cat > opt_${PID}.opts <<EOF
#include "$ROOTCNVROOT/options/Setup.opts"
ApplicationMgr.TopAlg      = { "ReadAlg" };
ReadAlg.OutputLevel        = 4;
EventSelector.FirstEvent   = 1;
EventSelector.Input        = { "DATAFILE='PFN:${DATA_DIR}/ROOTIO_${PID}.dst' SVC='Gaudi::RootEvtSelector'" };
RootCnvSvc.IOPerfStats     = "Read.100.ioperf.${PID}.${1}.root";
EOF
#
echo "Read ROOTIO to ${1} $NEVT events" >> ${LOG_FILE}
flush_disk ${1}
run opt_${PID}.opts >> ${LOG_FILE}
cat opt_${PID}.opts.log | grep "ReadAlg:execute" >> ${LOG_FILE}
echo " " >> ${LOG_FILE}
#
# =============================================================================================
#
cat > opt_${PID}.opts <<EOF
#include "$ROOTCNVROOT/options/Setup.opts"
ApplicationMgr.TopAlg = { "EvtCollectionWrite/Writer" };
ApplicationMgr.ExtSvc += { "TagCollectionSvc/EvtTupleSvc" };
ReadAlg.OutputLevel        = 4;
Writer.OutputLevel         = 4;
Writer.NumMcTracks         = 1;
EventSelector.Input        = {"DATAFILE='PFN:${DATA_DIR}/ROOTIO_${PID}.dst' SVC='Gaudi::RootEvtSelector'"};
EvtTupleSvc.Output         = {"EvtColl DATAFILE='PFN:${DATA_DIR}/ROOTIO_${PID}.tags' OPT='RECREATE' SVC='Gaudi::RootCnvSvc'" };
EOF
echo "CollWrite ROOTIO to ${1} $NEVT events" >> ${LOG_FILE}
flush_disk ${1}
run opt_${PID}.opts >> ${LOG_FILE}
cat opt_${PID}.opts.log | grep "ReadAlg:execute" >> ${LOG_FILE}
echo " " >> ${LOG_FILE}
#
# =============================================================================================
#
cat > opt_${PID}.opts <<EOF
#include "$ROOTCNVROOT/options/Setup.opts"
ApplicationMgr.TopAlg      = { "ReadAlg"};
ApplicationMgr.ExtSvc     += { "TagCollectionSvc/EvtTupleSvc" };
EventSelector.Input = {"COLLECTION='Dir1/Dir2/Dir3/Collection' DATAFILE='PFN:${DATA_DIR}/ROOTIO_${PID}.tags' SVC='Gaudi::RootCnvSvc' SEL='(Ntrack>10)' FUN='Gaudi::Examples::EvtCollectionSelector'"};
RootCnvSvc.IOPerfStats     = "Read.090.ioperf.${PID}.${1}.root";
EOF
echo "CollRead 90% ROOTIO to ${1} $NEVT events" >> ${LOG_FILE}
flush_disk ${1}
run opt_${PID}.opts >> ${LOG_FILE}
cat opt_${PID}.opts.log | grep "ReadAlg:execute" >> ${LOG_FILE}
echo " " >> ${LOG_FILE}
#
# =============================================================================================
#
cat > opt_${PID}.opts <<EOF
#include "$ROOTCNVROOT/options/Setup.opts"
ApplicationMgr.TopAlg      = { "ReadAlg"};
ApplicationMgr.ExtSvc     += { "TagCollectionSvc/EvtTupleSvc" };
EventSelector.Input = {"COLLECTION='Dir1/Dir2/Dir3/Collection' DATAFILE='PFN:${DATA_DIR}/ROOTIO_${PID}.tags' SVC='Gaudi::RootCnvSvc' SEL='(Ntrack>50)' FUN='Gaudi::Examples::EvtCollectionSelector'"};
RootCnvSvc.IOPerfStats     = "Read.050.ioperf.${PID}.${1}.root";
EOF
echo "CollRead 50% ROOTIO to ${1} $NEVT events" >> ${LOG_FILE}
flush_disk ${1}
run opt_${PID}.opts >> ${LOG_FILE}
cat opt_${PID}.opts.log | grep "ReadAlg:execute" >> ${LOG_FILE}
echo " " >> ${LOG_FILE}
#
# =============================================================================================
#
cat > opt_${PID}.opts <<EOF
#include "$ROOTCNVROOT/options/Setup.opts"
ApplicationMgr.TopAlg      = { "ReadAlg"};
ApplicationMgr.ExtSvc     += { "TagCollectionSvc/EvtTupleSvc" };
EventSelector.Input = {"COLLECTION='Dir1/Dir2/Dir3/Collection' DATAFILE='PFN:${DATA_DIR}/ROOTIO_${PID}.tags' SVC='Gaudi::RootCnvSvc' SEL='(Ntrack>90)' FUN='Gaudi::Examples::EvtCollectionSelector'"};
RootCnvSvc.IOPerfStats     = "Read.010.ioperf.${PID}.${1}.root";
EOF
echo "CollRead 10% ROOTIO to ${1} $NEVT events" >> ${LOG_FILE}
flush_disk ${1}
run opt_${PID}.opts >> ${LOG_FILE}
cat opt_${PID}.opts.log | grep "ReadAlg:execute" >> ${LOG_FILE}
echo " " >> ${LOG_FILE}

}


runtests file   ${DISK}
rm -f ${DISK}/ROOTIO_${PID}.dst
rm -f ${DISK}/ROOTIO_${PID}.tags
runtests castor /castor/cern.ch/user/f/frankb/data
rfrm /castor/cern.ch/user/f/frankb/data/ROOTIO_${PID}.dst
rfrm /castor/cern.ch/user/f/frankb/data/ROOTIO_${PID}.tags



cat ${LOG_FILE}
