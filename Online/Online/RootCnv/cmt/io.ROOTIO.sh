#!/bin/bash
export PID=$$
export NEVT=10000
export NEVT=19000
export LOG_FILE=IOExample.ROOTIO.${PID}.txt
export DISK=/build/${USER}
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
cat > read_pool_${PID}.opts <<EOF
#pragma print off
#include "$ROOTCNVROOT/options/SetupIO.opts"
ApplicationMgr.ExtSvc      = { "AuditorSvc","Gaudi::RootCnvSvc/RootCnvSvc" };
#include "$IOEXAMPLEROOT/options/ReadMin.opts"
#pragma print off
StoreExplorerAlg.PrintFreq     = 0.01;
StoreExplorerAlg.OutputLevel   = 4;
DumpMCEventAlg.OutputLevel     = 4;
EventSelector.PrintFreq        = 500;
FileCatalog.Catalogs           = { "xmlcatalog_file:ioExample.xml" } ;
ApplicationMgr.AuditAlgorithms = True;
ApplicationMgr.ExtSvc         += { "AuditorSvc"} ;
AuditorSvc.Auditors           += { "ChronoAuditor" };
ApplicationMgr.EvtMax          = ${NEVT};
PrintHeader.OutputLevel        = 4;
EventSelector.PrintFreq        = 100;
StoreExplorerAlg.AccessForeign = 1;
StoreExplorerAlg.Load          = 1;
StoreExplorerAlg.PrintEvt      = 100000;
StoreExplorerAlg.PrintFreq     = 1.;
StoreExplorerAlg.OutputLevel   = 4;
//RootCnvSvc.CacheSize         = -1;
RootCnvSvc.CacheSize           = 50000000;
RootCnvSvc.OutputLevel         = 3;
EOF
# =============================================================================================
#
cat > opt_${PID}.opts <<EOF
#include "$ROOTCNVROOT/options/SetupIO.opts"
#include "$GAUDIPOOLDBROOT/options/GaudiPoolDbRoot.opts"
ApplicationMgr.OutStream = {"InputCopyStream/InputCopyStream"};
FileCatalog.Catalogs     = {"xmlcatalog_file:ioExample.xml"};
InputCopyStream.Output   = "DATAFILE='PFN:${DATA_DIR}/ROOTIO_00005776_00000001_1.dst' SVC='RootCnvSvc' OPT='REC'";
EventSelector.Input      ={"DATAFILE='PFN:${DISK}/00005776_00000001_1.dst' TYP='POOL_ROOTTREE' OPT='READ'" };
ApplicationMgr.EvtMax    = ${NEVT};
EOF
#
if test "a" = ""
then
    flush_disk ${1}
    echo "Write ROOTIO to ${1} $NEVT events" >> ${LOG_FILE};
    run opt_${PID}.opts >> ${LOG_FILE};
    cat opt_${PID}.opts.log | grep "Ave/Min/Max" | grep "InputCopyStream" >> ${LOG_FILE};
    echo " " >> ${LOG_FILE};
    cat opt_${PID}.opts.log;
fi;
#
# =============================================================================================
#
cat > opt_${PID}.opts <<EOF
#include "read_pool_${PID}.opts"
EventSelector.Input = {"DATAFILE='PFN:${DATA_DIR}/ROOTIO_00005776_00000001_1.dst' SVC='Gaudi::RootEvtSelector' OPT='READ'"};
RootCnvSvc.IOPerfStats     = "Read.100.ioperf.${PID}.${1}.root";
EOF
#
if test "a" = ""
then
    flush_disk ${1}
    echo "Read ROOTIO to ${1} $NEVT events" >> ${LOG_FILE};
    run opt_${PID}.opts >> ${LOG_FILE};
    cat opt_${PID}.opts.log  | grep "Ave/Min/Max" | grep "ExampleSeq:execute" >> ${LOG_FILE}
    echo " " >> ${LOG_FILE}
    cat opt_${PID}.opts.log
##exit 1;
fi;
#
# =============================================================================================
#
cat > opt_${PID}.opts <<EOF
#include "read_pool_${PID}.opts"
EventSelector.Input = {"DATAFILE='PFN:${DATA_DIR}/ROOTIO_00005776_00000001_1.dst' SVC='Gaudi::RootEvtSelector' OPT='READ'"};
ApplicationMgr.ExtSvc += { "TagCollectionSvc/EvtTupleSvc" };
ApplicationMgr.TopAlg += {"DstCollectionWrite"};
EvtTupleSvc.Output = {"EvtColl DATAFILE='PFN:${DATA_DIR}/ROOTIO_${PID}.tags' OPT='RECREATE' SVC='Gaudi::RootCnvSvc'" };
EOF
#
echo "CollWrite ROOTIO to ${1} $NEVT events" >> ${LOG_FILE}
if test "a" = ""
then
    flush_disk ${1}
    run opt_${PID}.opts >> ${LOG_FILE};
    cat opt_${PID}.opts.log  | grep "Ave/Min/Max" | grep "ExampleSeq:execute" >> ${LOG_FILE};
    echo " " >> ${LOG_FILE}
    cat opt_${PID}.opts.log
else
    if test "${1}" != "castor"
	then 
	ln -s ${DISK}/ROOTIO.tags ${DISK}/ROOTIO_${PID}.tags;
    fi;
fi;
#
# =============================================================================================
#
cat > opt_${PID}.opts <<EOF
#include "read_pool_${PID}.opts"
ApplicationMgr.ExtSvc += { "TagCollectionSvc/EvtTupleSvc" };
EventSelector.Input = {"COLLECTION='Dir1/Collection' DATAFILE='PFN:${DATA_DIR}/ROOTIO.tags' SVC='Gaudi::RootCnvSvc' SEL='(Collisions>4)'"};
EventSelector.Input = {"COLLECTION='Dir1/Collection' DATAFILE='PFN:${DATA_DIR}/ROOTIO.tags' SVC='Gaudi::RootCnvSvc' SEL='(PtHat>85.)'"};
RootCnvSvc.IOPerfStats     = "Read.010.ioperf.${PID}.${1}.root";
ApplicationMgr.TopAlg += {"StoreSnifferAlg"};
RootCnvSvc.CacheSize       = 500;
EOF
#
if test "a" = "a"
then
    #flush_disk ${1}
    echo "CollRead 10% ROOTIO to ${1} $NEVT events" >> ${LOG_FILE};
    run opt_${PID}.opts >> ${LOG_FILE};
    cat opt_${PID}.opts.log  | grep "Ave/Min/Max" | grep "ExampleSeq:execute" >> ${LOG_FILE}
    echo " " >> ${LOG_FILE}
    cat opt_${PID}.opts.log
fi;
#
# =============================================================================================
#
cat > opt_${PID}.opts <<EOF
#include "read_pool_${PID}.opts"
ApplicationMgr.ExtSvc += { "TagCollectionSvc/EvtTupleSvc" };
EventSelector.Input = {"COLLECTION='Dir1/Collection' DATAFILE='PFN:${DATA_DIR}/ROOTIO_${PID}.tags' SVC='Gaudi::RootCnvSvc' SEL='(Collisions>3 || Collisions<2)'"};
RootCnvSvc.IOPerfStats     = "Read.050.ioperf.${PID}.${1}.root";
EOF
#
if test "a" = ""
then
    flush_disk ${1}
    echo "CollRead 50% ROOTIO to ${1} $NEVT events" >> ${LOG_FILE};
    run opt_${PID}.opts >> ${LOG_FILE};
    cat opt_${PID}.opts.log  | grep "Ave/Min/Max" | grep "ExampleSeq:execute" >> ${LOG_FILE}
    echo " " >> ${LOG_FILE}
    cat opt_${PID}.opts.log
fi;
#
# =============================================================================================
#
cat > opt_${PID}.opts <<EOF
#include "read_pool_${PID}.opts"
ApplicationMgr.ExtSvc += { "TagCollectionSvc/EvtTupleSvc" };
EventSelector.Input = {"COLLECTION='Dir1/Collection' DATAFILE='PFN:${DATA_DIR}/ROOTIO_${PID}.tags' SVC='Gaudi::RootCnvSvc' SEL='(Collisions<4)'"};
RootCnvSvc.IOPerfStats     = "Read.090.ioperf.${PID}.${1}.root";
EOF
#
if test "a" = ""
then
    flush_disk ${1}
    echo "CollRead 90% ROOTIO to ${1} $NEVT events" >> ${LOG_FILE};
    run opt_${PID}.opts >> ${LOG_FILE};
    cat opt_${PID}.opts.log  | grep "Ave/Min/Max" | grep "ExampleSeq:execute" >> ${LOG_FILE}
    echo " " >> ${LOG_FILE}
    cat opt_${PID}.opts.log
fi;
}

#rfcp /castor/cern.ch/user/f/frankb/data/00005776_00000001_1.dst ${DISK}/00005776_00000001_1.dst
#cp ${DISK}/00005776_00000001_1.dst ${DISK}/ROOTIO_00005776_00000001_1.dst
#runtests file   ${DISK}
runtests castor /castor/cern.ch/user/f/frankb/data


cat ${LOG_FILE}
