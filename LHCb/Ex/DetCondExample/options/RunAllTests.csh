#!/usr/local/bin/tcsh -f 
# $Id: RunAllTests.csh,v 1.2 2002-12-03 17:16:42 andreav Exp $
if ( 1 != ${?DETCONDEXAMPLEROOT} ) then
  echo "ERROR! Must 'source setup.csh -tag=$CMTDEB' before running this script"
  exit 1
endif
set echo
cd ${DETCONDEXAMPLEROOT}/options

mkdir -p ../job/NEWlog.${CONDDB_implementation}
rm -f ../job/NEWlog.${CONDDB_implementation}/log.*.${CMTCONFIG}

../$CMTDEB/DetCondExample.exe testXml.opts \
  | sed "s|$DETCONDEXAMPLEROOT|DETCONDEXAMPLEROOT|" \
  > ../job/NEWlog.${CONDDB_implementation}/log.testXml.${CMTCONFIG}

source ../cmt/cleanupDB.csh -tag=$CMTDEB
source ../cmt/setupDB.csh   -tag=$CMTDEB
../$CMTDEB/DetCondExample.exe fillCondDB.opts \
  | sed "s|$DETCONDEXAMPLEROOT|DETCONDEXAMPLEROOT|" \
  > ../job/NEWlog.${CONDDB_implementation}/log.fillCondDB_1.${CMTCONFIG}
../$CMTDEB/DetCondExample.exe fillCondDB.opts \
  | sed "s|$DETCONDEXAMPLEROOT|DETCONDEXAMPLEROOT|" \
  > ../job/NEWlog.${CONDDB_implementation}/log.fillCondDB_2.${CMTCONFIG}

../$CMTDEB/DetCondExample.exe testCondDB.opts \
  | sed "s|$DETCONDEXAMPLEROOT|DETCONDEXAMPLEROOT|" \
  > ../job/NEWlog.${CONDDB_implementation}/log.testCondDB.${CMTCONFIG}

../$CMTDEB/DetCondExample.exe daqCondDB.opts \
  | sed "s|$DETCONDEXAMPLEROOT|DETCONDEXAMPLEROOT|" \
  > ../job/NEWlog.${CONDDB_implementation}/log.daqCondDB.${CMTCONFIG}

#-----------------------------------------------------------------------------


