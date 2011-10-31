#!/bin/bash
export PID=$$
export NEVT=10000
export NEVT=20000
export NEVT=20000

export LOG_FILE=IOExample.ROOTIO.${PID}.txt
export DISK=/build/${USER}
export DISK=/opt/dirac/${USER}
export DISK=castor:/castor/cern.ch/user/f/frankb
###export DISK=/tmp/${USER}
echo `date` > ${LOG_FILE}

cd /afs/cern.ch/user/f/frankb/cmtuser/LHCb_v33r0/Online/RootCnv/cmt
. `which SetupProject.sh` LHCb v33r0
export CMTCONFIG=$CMTOPT
. setup.sh

export LD_PRELOAD=/afs/cern.ch/sw/Gaudi/releases/GAUDI/GAUDI_v22r4/GaudiKernel/x86_64-slc5-gcc43-opt/libGaudiKernelDict.so
#
#
#
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
    ##sudo /afs/cern.ch/user/f/frankb/cmtuser/Gaudi_v21r7/Online/RootCnv/cmt/flush_caches.sh;
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
#include "$ROOTCNVROOT/options/ReadMin.opts"
#pragma print off
ApplicationMgr.EvtMax          = ${NEVT};
EventSelector.PrintFreq        = 100;
StoreExplorerAlg.PrintFreq     = 1.;
StoreExplorerAlg.OutputLevel   = 4;
//RootCnvSvc.CacheSize         = -1;
RootCnvSvc.CacheSize           = 50000000;
RootCnvSvc.OutputLevel         = 3;
FileCatalog.Catalogs           = {"xmlcatalog_file:ioExample.xml"};
EOF
# =============================================================================================
#
export INPUT_FILE=${DATA_DIR}/ROOTIO_00010911_00000001_5M.dst
cat > write_${PID}.opts <<EOF
#include "$ROOTCNVROOT/options/SetupIO.opts"
RootCnvSvc.CacheSize     = 50000000;
RootCnvSvc.BasketSize    = 20000000;
ApplicationMgr.OutStream = {"InputCopyStream/InputCopyStream"};
FileCatalog.Catalogs     = {"xmlcatalog_file:ioExample.xml"};
InputCopyStream.Output   = "DATAFILE='PFN:${INPUT_FILE}' SVC='RootCnvSvc' OPT='REC'";
EventSelector.Input      ={"DATAFILE='PFN:${DISK}/00010911_00000001_1.dst' SVC='Gaudi::RootEvtSelector' OPT='READ'" };
ApplicationMgr.EvtMax    = ${NEVT};
EOF
#
if test "a" = ""
then
    flush_disk ${1}
    echo "Write ROOTIO to ${1} $NEVT events" >> ${LOG_FILE};
    run write_${PID}.opts >> ${LOG_FILE};
    cat write_${PID}.opts.log | grep "Ave/Min/Max" | grep "InputCopyStream" >> ${LOG_FILE};
    echo " " >> ${LOG_FILE};
    cat write_${PID}.opts.log;
fi;
#
# =============================================================================================
#
cat > readall_${PID}.opts <<EOF
#include "read_pool_${PID}.opts"
EventSelector.Input = {"DATAFILE='PFN:${INPUT_FILE}' SVC='Gaudi::RootEvtSelector' OPT='READ'"};
RootCnvSvc.IOPerfStats = "Read.100.ioperf.${PID}.${1}.root";
EOF
#
if test "a" = "a"
then
    flush_disk ${1}
    echo "Read ROOTIO to ${1} $NEVT events" >> ${LOG_FILE};
    run readall_${PID}.opts >> ${LOG_FILE};
    cat readall_${PID}.opts.log  | grep "Ave/Min/Max" | grep "ExampleSeq:execute" >> ${LOG_FILE}
    echo " " >> ${LOG_FILE}
    cat readall_${PID}.opts.log
##exit 1;
fi;
#
# =============================================================================================
#
cat > collwrt_${PID}.opts <<EOF
#include "read_pool_${PID}.opts"
EventSelector.Input = {"DATAFILE='PFN:${INPUT_FILE}' SVC='Gaudi::RootEvtSelector' OPT='READ'"};
ApplicationMgr.ExtSvc += { "TagCollectionSvc/EvtTupleSvc" };
ApplicationMgr.TopAlg += { "DstCollectionWrite" };
EvtTupleSvc.Output = {"EvtColl DATAFILE='PFN:${DATA_DIR}/ROOTIO_${PID}.tags' OPT='RECREATE' SVC='Gaudi::RootCnvSvc'" };
EOF
#
echo "CollWrite ROOTIO to ${1} $NEVT events" >> ${LOG_FILE}
if test "a" = "a"
then
    flush_disk ${1}
    run collwrt_${PID}.opts >> ${LOG_FILE};
    cat collwrt_${PID}.opts.log  | grep "Ave/Min/Max" | grep "ExampleSeq:execute" >> ${LOG_FILE};
    echo " " >> ${LOG_FILE}
    cat collwrt_${PID}.opts.log
else
    if test "${1}" != "castor"
	then 
	ln -s ${DISK}/ROOTIO.tags ${DISK}/ROOTIO_${PID}.tags;
    fi;
fi;
#
# =============================================================================================
#
cat > read010_${PID}.opts <<EOF
#include "read_pool_${PID}.opts"
ApplicationMgr.ExtSvc += { "TagCollectionSvc/EvtTupleSvc" };
EventSelector.Input = {"COLLECTION='Dir1/Collection' DATAFILE='PFN:${DATA_DIR}/ROOTIO_${PID}.tags' SVC='Gaudi::RootCnvSvc' SEL='(Collisions>4)'"};
EventSelector.Input = {"COLLECTION='Dir1/Collection' DATAFILE='PFN:${DATA_DIR}/ROOTIO_${PID}.tags' SVC='Gaudi::RootCnvSvc' SEL='(PtHat>32.5)'"};
RootCnvSvc.IOPerfStats     = "Read.010.ioperf.${PID}.${1}.root";
ApplicationMgr.TopAlg += {"StoreSnifferAlg"};
RootCnvSvc.CacheSize       = 500;
EOF
#
if test "a" = ""
then
    #flush_disk ${1}
    echo "CollRead 10% ROOTIO to ${1} $NEVT events" >> ${LOG_FILE};
    run read010_${PID}.opts >> ${LOG_FILE};
    cat read010_${PID}.opts.log  | grep "Ave/Min/Max" | grep "ExampleSeq:execute" >> ${LOG_FILE}
    echo " " >> ${LOG_FILE}
    cat read010_${PID}.opts.log
fi;
#
# =============================================================================================
#
cat > read050_${PID}.opts <<EOF
#include "read_pool_${PID}.opts"
ApplicationMgr.ExtSvc += { "TagCollectionSvc/EvtTupleSvc" };
EventSelector.Input = {"COLLECTION='Dir1/Collection' DATAFILE='PFN:${DATA_DIR}/ROOTIO_${PID}.tags' SVC='Gaudi::RootCnvSvc' SEL='(PtHat<18)'"};
RootCnvSvc.IOPerfStats     = "Read.050.ioperf.${PID}.${1}.root";
EOF
#
if test "a" = ""
then
    flush_disk ${1}
    echo "CollRead 50% ROOTIO to ${1} $NEVT events" >> ${LOG_FILE};
    run read050_${PID}.opts >> ${LOG_FILE};
    cat read050_${PID}.opts.log  | grep "Ave/Min/Max" | grep "ExampleSeq:execute" >> ${LOG_FILE}
    echo " " >> ${LOG_FILE}
    cat read050_${PID}.opts.log
fi;
#
# =============================================================================================
#
cat > read090_${PID}.opts <<EOF
#include "read_pool_${PID}.opts"
ApplicationMgr.ExtSvc += { "TagCollectionSvc/EvtTupleSvc" };
EventSelector.Input = {"COLLECTION='Dir1/Collection' DATAFILE='PFN:${DATA_DIR}/ROOTIO_${PID}.tags' SVC='Gaudi::RootCnvSvc' SEL='(PtHat<32.5)'"};
RootCnvSvc.IOPerfStats     = "Read.090.ioperf.${PID}.${1}.root";
EOF
#
if test "a" = ""
then
    flush_disk ${1}
    echo "CollRead 90% ROOTIO to ${1} $NEVT events" >> ${LOG_FILE};
    run read090_${PID}.opts >> ${LOG_FILE};
    cat read090_${PID}.opts.log  | grep "Ave/Min/Max" | grep "ExampleSeq:execute" >> ${LOG_FILE}
    echo " " >> ${LOG_FILE}
    cat read090_${PID}.opts.log
fi;
}

#rfcp /castor/cern.ch/user/f/frankb/data/00010911_00000001_1.dst ${DISK}/00010911_00000001_1.dst
#cp ${DISK}/00005776_00000001_1.dst ${DISK}/ROOTIO_00005776_00000001_1.dst
#runtests file   ${DISK}

###rfcp /castor/cern.ch/grid/lhcb/LHCb/Collision11/ALLSTREAMS.DST/00011749/0000/00010911_00000001_1.dst.allstreams.dst /castor/cern.ch/user/f/frankb/00010911_00000001_1.dst.allstreams.dst

runtests castor /castor/cern.ch/user/f/frankb/data


cat ${LOG_FILE}
