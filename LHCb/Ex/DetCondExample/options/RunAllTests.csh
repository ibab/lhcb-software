#!/usr/local/bin/tcsh -f 
# $Id: RunAllTests.csh,v 1.1 2002-12-02 11:37:05 andreav Exp $
if ( 1 != ${?DETCONDEXAMPLEROOT} ) then
  echo "ERROR! Must 'source setup.csh -tag=$CMTDEB' before running this script"
  exit 1
endif
set echo
cd ${DETCONDEXAMPLEROOT}/options
../sql/dropCondDB.csh

/bin/rm -f log.testXml.${CMTCONFIG}
../$CMTDEB/DetCondExample.exe testXml.opts \
  | sed "s|$DETCONDEXAMPLEROOT|DETCONDEXAMPLEROOT|" > log.testXml.${CMTCONFIG}

source ../cmt/cleanupDB.csh -tag=$CMTDEB
source ../cmt/setupDB.csh   -tag=$CMTDEB
/bin/rm -f log.fillCondDB_1.${CMTCONFIG}
/bin/rm -f log.fillCondDB_2.${CMTCONFIG}
../$CMTDEB/DetCondExample.exe fillCondDB.opts \
  | sed "s|$DETCONDEXAMPLEROOT|DETCONDEXAMPLEROOT|" \
  > log.fillCondDB_1.${CMTCONFIG}
../$CMTDEB/DetCondExample.exe fillCondDB.opts \
  | sed "s|$DETCONDEXAMPLEROOT|DETCONDEXAMPLEROOT|" \
  > log.fillCondDB_2.${CMTCONFIG}

/bin/rm -f log.testCondDB.${CMTCONFIG}
../$CMTDEB/DetCondExample.exe testCondDB.opts \
  | sed "s|$DETCONDEXAMPLEROOT|DETCONDEXAMPLEROOT|" \
  > log.testCondDB.${CMTCONFIG}

/bin/rm -f log.daqCondDB.Objy.Linux61
../$CMTDEB/DetCondExample.exe daqCondDB.opts \
  | sed "s|$DETCONDEXAMPLEROOT|DETCONDEXAMPLEROOT|" \
  > log.daqCondDB.${CMTCONFIG}

#-----------------------------------------------------------------------------


