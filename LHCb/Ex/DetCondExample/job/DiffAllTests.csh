#!/usr/local/bin/tcsh -f 
# $Id: DiffAllTests.csh,v 1.1 2002-12-03 17:18:29 andreav Exp $
if ( 1 != ${?DETCONDEXAMPLEROOT} ) then
  echo "ERROR! Must 'source setup.csh -tag=$CMTDEB' before running this script"
  exit 1
endif
cd ${DETCONDEXAMPLEROOT}/options
dos2unix log.*.Win32Debug >& /dev/null

set theDiff = diff.${CMTCONFIG}
rm -f $theDiff 
touch $theDiff
diff log.testXml.${CMTCONFIG} ../job/Xml/ >> $theDiff
diff log.fillCondDB_1.${CMTCONFIG} ../job/${CONDDB_implementation}/ >> $theDiff
diff log.fillCondDB_2.${CMTCONFIG} ../job/${CONDDB_implementation}/ >> $theDiff
diff log.testCondDB.${CMTCONFIG} ../job/${CONDDB_implementation}/ >> $theDiff
diff log.daqCondDB.${CMTCONFIG} ../job/${CONDDB_implementation}/ >> $theDiff
unset theDiff

set theDiff = diff.Win32Debug
rm -f $theDiff 
touch $theDiff
diff log.testXml.Win32Debug ../job/Xml/ >> $theDiff
diff log.fillCondDB_1.Win32Debug ../job/${CONDDB_implementation}/ >> $theDiff
diff log.fillCondDB_2.Win32Debug ../job/${CONDDB_implementation}/ >> $theDiff
diff log.testCondDB.Win32Debug ../job/${CONDDB_implementation}/ >> $theDiff
diff log.daqCondDB.Win32Debug ../job/${CONDDB_implementation}/ >> $theDiff
unset theDiff

set theDiff = diff.Lin-vs-Win
rm -f $theDiff 
touch $theDiff
diff log.testXml.${CMTCONFIG} log.testXml.Win32Debug >> $theDiff
diff log.fillCondDB_1.${CMTCONFIG} log.fillCondDB_1.Win32Debug >> $theDiff
diff log.fillCondDB_2.${CMTCONFIG} log.fillCondDB_2.Win32Debug >> $theDiff
diff log.testCondDB.${CMTCONFIG} log.testCondDB.Win32Debug >> $theDiff
diff log.daqCondDB.${CMTCONFIG} log.daqCondDB.Win32Debug >> $theDiff
unset theDiff
