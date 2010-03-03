#!/bin/bash
export PID=$$
export NEVT=100000
export DISK=/tmp/${USER}
export DISK=/opt/dirac/${USER}
export LOG_FILE=IOExample.POOLIO.${PID}.txt
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
cat > read_pool_${PID}.opts <<EOF
#pragma print off
#include "$IOEXAMPLEROOT/options/ReadMin.opts"
DumpMCEventAlg.OutputLevel = 4;
EventSelector.PrintFreq    = 500;
FileCatalog.Catalogs  = { "xmlcatalog_file:ioExample_${PID}.xml" } ;
ApplicationMgr.AuditAlgorithms = True;
ApplicationMgr.ExtSvc += { "AuditorSvc"} ;
AuditorSvc.Auditors   += { "ChronoAuditor" };
ApplicationMgr.EvtMax = ${NEVT};
PrintHeader.OutputLevel = 4;
EventSelector.PrintFreq    = 500;
StoreExplorerAlg.Load          = 1;
StoreExplorerAlg.PrintEvt      = 100000;
StoreExplorerAlg.PrintFreq     = 1.;
StoreExplorerAlg.OutputLevel   = 4;
EOF
# =============================================================================================
#
cat > opt_${PID}.opts <<EOF
#include "$ROOTCNVROOT/options/SetupIO.opts"
#include "$GAUDIPOOLDBROOT/options/GaudiPoolDbRoot.opts"
ApplicationMgr.OutStream = {"InputCopyStream/InputCopyStream"};
FileCatalog.Catalogs     = {"xmlcatalog_file:ioExample_${PID}.xml"};
InputCopyStream.Output   = "DATAFILE='PFN:${DATA_DIR}/POOLIO_00005776_00000001_1.dst' TYP='POOL_ROOTTREE' OPT='REC'";
EventSelector.Input      ={"DATAFILE='PFN:${DISK}/00005776_00000001_1.dst' TYP='POOL_ROOTTREE' OPT='READ'" };
ApplicationMgr.EvtMax    = ${NEVT};
EOF
#
echo "Write POOLIO to ${1} $NEVT events" >> ${LOG_FILE}
run opt_${PID}.opts >> ${LOG_FILE}
cat opt_${PID}.opts.log | grep "Ave/Min/Max" | grep "InputCopyStream" >> ${LOG_FILE}
echo " " >> ${LOG_FILE}
cat opt_${PID}.opts.log
#
# =============================================================================================
#
if test "${1}" = "castor"
then
  rm -f ioExample_${PID}.xml
  rfrm /castor/cern.ch/user/f/frankb/data/POOLIO_00005776_00000001_1.dst
  rfcp /castor/cern.ch/user/f/frankb/data/00005776_00000001_1.dst /castor/cern.ch/user/f/frankb/data/POOLIO_00005776_00000001_1.dst
  root.exe -l xmlCatalog.C\(\"ioExample_${PID}.xml\",\"castor:/castor/cern.ch/user/f/frankb/data/POOLIO_00005776_00000001_1.dst\"\)
  cat ioExample_${PID}.xml
fi;
#
# =============================================================================================
#
cat > opt_${PID}.opts <<EOF
#include "read_pool_${PID}.opts"
EventSelector.Input = {"DATAFILE='PFN:${DATA_DIR}/POOLIO_00005776_00000001_1.dst' TYP='POOL_ROOTTREE' OPT='READ'"};
EOF
#
flush_disk ${1};
echo "Read POOLIO to ${1} $NEVT events" >> ${LOG_FILE}
run opt_${PID}.opts >> ${LOG_FILE}
cat opt_${PID}.opts.log  | grep "Ave/Min/Max" | grep "ExampleSeq:execute" >> ${LOG_FILE}
echo " " >> ${LOG_FILE}
cat opt_${PID}.opts.log
#
# =============================================================================================
#
cat > opt_${PID}.opts <<EOF
#include "read_pool_${PID}.opts"
EventSelector.Input = {"DATAFILE='PFN:${DATA_DIR}/POOLIO_00005776_00000001_1.dst' TYP='POOL_ROOTTREE' OPT='READ'"};
ApplicationMgr.ExtSvc += { "TagCollectionSvc/EvtTupleSvc" };
ApplicationMgr.TopAlg += {"DstCollectionWrite"};
EvtTupleSvc.Output = {"EvtColl DATAFILE='PFN:${DATA_DIR}/POOLIO_${PID}.tags' OPT='RECREATE' TYP='POOL_ROOTTREE'" };
EOF
#
echo "CollWrite POOLIO to ${1} $NEVT events" >> ${LOG_FILE}
run opt_${PID}.opts >> ${LOG_FILE}
cat opt_${PID}.opts.log  | grep "Ave/Min/Max" | grep "ExampleSeq:execute" >> ${LOG_FILE}
echo " " >> ${LOG_FILE}
cat opt_${PID}.opts.log
#
# =============================================================================================
#
cat > opt_${PID}.opts <<EOF
#include "read_pool_${PID}.opts"
ApplicationMgr.ExtSvc += { "TagCollectionSvc/EvtTupleSvc" };
EventSelector.Input = {"COLLECTION='Dir1/Collection' DATAFILE='PFN:${DATA_DIR}/POOLIO_${PID}.tags' TYP='POOL_ROOT' SEL='(Collisions>3)'"};
EOF
#
flush_disk ${1};
echo "CollRead 10% POOLIO to ${1} $NEVT events" >> ${LOG_FILE}
run opt_${PID}.opts >> ${LOG_FILE}
cat opt_${PID}.opts.log  | grep "Ave/Min/Max" | grep "ExampleSeq:execute" >> ${LOG_FILE}
echo " " >> ${LOG_FILE}
cat opt_${PID}.opts.log
#
# =============================================================================================
#
cat > opt_${PID}.opts <<EOF
#include "read_pool_${PID}.opts"
ApplicationMgr.ExtSvc += { "TagCollectionSvc/EvtTupleSvc" };
EventSelector.Input = {"COLLECTION='Dir1/Collection' DATAFILE='PFN:${DATA_DIR}/POOLIO_${PID}.tags' TYP='POOL_ROOT' SEL='(Collisions>3 || Collisions<2)'"};
EOF
#
flush_disk ${1};
echo "CollRead 50% POOLIO to ${1} $NEVT events" >> ${LOG_FILE}
run opt_${PID}.opts >> ${LOG_FILE}
cat opt_${PID}.opts.log  | grep "Ave/Min/Max" | grep "ExampleSeq:execute" >> ${LOG_FILE}
echo " " >> ${LOG_FILE}
cat opt_${PID}.opts.log
#
# =============================================================================================
#
cat > opt_${PID}.opts <<EOF
#include "read_pool_${PID}.opts"
ApplicationMgr.ExtSvc += { "TagCollectionSvc/EvtTupleSvc" };
EventSelector.Input = {"COLLECTION='Dir1/Collection' DATAFILE='PFN:${DATA_DIR}/POOLIO_${PID}.tags' TYP='POOL_ROOT' SEL='(Collisions<4)'"};
EOF
#
flush_disk ${1};
echo "CollRead 90% POOLIO to ${1} $NEVT events" >> ${LOG_FILE}
run opt_${PID}.opts >> ${LOG_FILE}
cat opt_${PID}.opts.log  | grep "Ave/Min/Max" | grep "ExampleSeq:execute" >> ${LOG_FILE}
echo " " >> ${LOG_FILE}
cat opt_${PID}.opts.log
}


####rfcp /castor/cern.ch/user/f/frankb/data/00005776_00000001_1.dst ${DISK}/00005776_00000001_1.dst
runtests castor /castor/cern.ch/user/f/frankb/data
runtests file   ${DISK}


cat ${LOG_FILE}
