#!/bin/bash
export PID=$$
export NEVT=500000
export NEVT=1000000
export DISK=/tmp/${USER}
export DISK=/opt/dirac/${USER}
export LOG_FILE=GaudiExample.POOLIO.${PID}.txt
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

runtests()
{
export DATA_DIR=${1}:${2}
echo "Data directory: ${DATA_DIR}" >> ${LOG_FILE}
#
cat > read_pool_${PID}.opts <<EOF
EventSelector.OutputLevel  = 2;
EventSelector.PrintFreq    = 500;
EventSelector.FirstEvent   = 1;
ApplicationMgr.EvtMax      = -1 ;
ApplicationMgr.Histogrampersistency = "NONE";
#include "GaudiPoolDbRoot.opts"
FileCatalog.Catalogs = { "xmlcatalog_file:POOLIO_${PID}.xml" } ;
ReadAlg.OutputLevel        = 4;
Writer.OutputLevel         = 4;
Writer.NumMcTracks         = 1;
PoolDbCacheSvc.OutputLevel = 4;
MessageSvc.OutputLevel     = 3;
ApplicationMgr.AuditAlgorithms = True ;
ApplicationMgr.ExtSvc     += { "AuditorSvc"} ;
AuditorSvc.Auditors       += { "ChronoAuditor" };
ChronoStatSvc.OutputLevel  = 4;
EOF
# =============================================================================================
#
cat > opt_${PID}.opts <<EOF
#include "read_pool_${PID}.opts"
ApplicationMgr.OutStream  += { "OutputStream/RootDst"};
ApplicationMgr.TopAlg      = { "WriteAlg", "ReadAlg" };
ApplicationMgr.TopAlg      = { "WriteAlg"};
ApplicationMgr.EvtSel      = "NONE"; // do not use any event input
RootDst.ItemList           = { "/Event#999" } ;
RootDst.Output             = "DATAFILE='PFN:${DATA_DIR}/POOLIO_${PID}.dst' TYP='POOL_ROOT' OPT='RECREATE'";
ApplicationMgr.EvtMax      = ${NEVT};
EOF
#
echo "Write POOLIO to ${1} $NEVT events" >> ${LOG_FILE}
run opt_${PID}.opts >> ${LOG_FILE}
cat opt_${PID}.opts.log | grep "RootDst:execute" >> ${LOG_FILE}
echo " " >> ${LOG_FILE}
cat opt_${PID}.opts.log
if test "${1}" = "castor"
then
  rfrm /castor/cern.ch/user/f/${USER}/data/POOLIO_${PID}.dst;
  rfcp ${DISK}/POOLIO_${PID}.dst /castor/cern.ch/user/f/${USER}/data/POOLIO_${PID}.dst;
  rm -f POOLIO_${PID}.xml
  root.exe -l xmlCatalog.C\(\"POOLIO_${PID}.xml\",\"castor:/castor/cern.ch/user/f/frankb/data/POOLIO_${PID}.dst\"\)
  cat POOLIO_${PID}.xml
fi;
#
# =============================================================================================
#
cat > opt_${PID}.opts <<EOF
#include "read_pool_${PID}.opts"
ApplicationMgr.TopAlg      = { "ReadAlg" };
EventSelector.Input        = { "DATAFILE='PFN:${DATA_DIR}/POOLIO_${PID}.dst' TYP='POOL_ROOT'" };
EOF
#
echo "Read POOLIO to ${1} $NEVT events" >> ${LOG_FILE}
run opt_${PID}.opts >> ${LOG_FILE}
cat opt_${PID}.opts.log | grep "ReadAlg:execute" >> ${LOG_FILE}
echo " " >> ${LOG_FILE}
cat opt_${PID}.opts.log
#
# =============================================================================================
#
cat > opt_${PID}.opts <<EOF
#include "read_pool_${PID}.opts"
ApplicationMgr.TopAlg = { "EvtCollectionWrite/Writer" };
ApplicationMgr.ExtSvc += { "TagCollectionSvc/EvtTupleSvc" };
EventSelector.Input        = {"DATAFILE='PFN:${DATA_DIR}/POOLIO_${PID}.dst' TYP='POOL_ROOT'"};
EvtTupleSvc.Output         = {"EvtColl DATAFILE='PFN:${DATA_DIR}/POOLIO_${PID}.tags' OPT='RECREATE' TYP='POOL_ROOTTREE'" };
EOF
echo "CollWrite POOLIO to ${1} $NEVT events" >> ${LOG_FILE}
run opt_${PID}.opts >> ${LOG_FILE}
cat opt_${PID}.opts.log | grep "ReadAlg:execute" >> ${LOG_FILE}
echo " " >> ${LOG_FILE}
cat opt_${PID}.opts.log
#
# =============================================================================================
#
cat > opt_${PID}.opts <<EOF
#include "read_pool_${PID}.opts"
ApplicationMgr.TopAlg      = { "ReadAlg"};
ApplicationMgr.ExtSvc     += { "TagCollectionSvc/EvtTupleSvc" };
EventSelector.Input = {"COLLECTION='Dir1/Dir2/Dir3/Collection' DATAFILE='PFN:${DATA_DIR}/POOLIO_${PID}.tags' TYP='POOL_ROOT' SEL='(Ntrack>10)' FUN='Gaudi::Examples::EvtCollectionSelector'"};
EOF
echo "CollRead 90% POOLIO to ${1} $NEVT events" >> ${LOG_FILE}
run opt_${PID}.opts >> ${LOG_FILE}
cat opt_${PID}.opts.log | grep "ReadAlg:execute" >> ${LOG_FILE}
echo " " >> ${LOG_FILE}
cat opt_${PID}.opts.log
#
# =============================================================================================
#
cat > opt_${PID}.opts <<EOF
#include "read_pool_${PID}.opts"
ApplicationMgr.TopAlg      = { "ReadAlg"};
ApplicationMgr.ExtSvc     += { "TagCollectionSvc/EvtTupleSvc" };
EventSelector.Input = {"COLLECTION='Dir1/Dir2/Dir3/Collection' DATAFILE='PFN:${DATA_DIR}/POOLIO_${PID}.tags' TYP='POOL_ROOT' SEL='(Ntrack>50)' FUN='Gaudi::Examples::EvtCollectionSelector'"};
EOF
echo "CollRead 50% POOLIO to ${1} $NEVT events" >> ${LOG_FILE}
run opt_${PID}.opts >> ${LOG_FILE}
cat opt_${PID}.opts.log | grep "ReadAlg:execute" >> ${LOG_FILE}
echo " " >> ${LOG_FILE}
cat opt_${PID}.opts.log
#
# =============================================================================================
#
cat > opt_${PID}.opts <<EOF
#include "read_pool_${PID}.opts"
ApplicationMgr.TopAlg      = { "ReadAlg"};
ApplicationMgr.ExtSvc     += { "TagCollectionSvc/EvtTupleSvc" };
EventSelector.Input = {"COLLECTION='Dir1/Dir2/Dir3/Collection' DATAFILE='PFN:${DATA_DIR}/POOLIO_${PID}.tags' TYP='POOL_ROOT' SEL='(Ntrack>90)' FUN='Gaudi::Examples::EvtCollectionSelector'"};
EOF
echo "CollRead 10% POOLIO to ${1} $NEVT events" >> ${LOG_FILE}
run opt_${PID}.opts >> ${LOG_FILE}
cat opt_${PID}.opts.log | grep "ReadAlg:execute" >> ${LOG_FILE}
echo " " >> ${LOG_FILE}
cat opt_${PID}.opts.log

rm -f read_pool_${PID}.opts
}


runtests file   ${DISK}


runtests castor /castor/cern.ch/user/f/frankb/data

rfrm /castor/cern.ch/user/f/frankb/data/POOLIO_${PID}.dst
rfrm /castor/cern.ch/user/f/frankb/data/POOLIO_${PID}.tags
rm -f ${DISK}/POOLIO_${PID}.dst
rm -f ${DISK}/POOLIO_${PID}.tags



cat ${LOG_FILE}
