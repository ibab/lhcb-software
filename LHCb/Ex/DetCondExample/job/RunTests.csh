#!/usr/local/bin/tcsh -f 
# $Id: RunTests.csh,v 1.1 2002-12-04 14:07:50 andreav Exp $
if ( 1 != ${?DETCONDEXAMPLEROOT} ) then
  echo "ERROR! You must 'source ../cmt/setup.csh -tag=$CMTDEB' first"
  exit 1
endif

if ( "${CMTCONFIG}" == "rh61_gcc2952" ) then
  set theImpls = "CondDBObjy"
else
  set theImpls = "CondDBOracle CondDBMySQL"
endif

foreach theImpl ( $theImpls )

  cd ${DETCONDEXAMPLEROOT}/options
  setenv CONDDB_implementation $theImpl
  source ../cmt/setup.csh -tag=$CMTDEB

  set echo
  mkdir -p ../job/NEWlog.${CMTCONFIG}.${CONDDB_implementation}
  rm -f    ../job/NEWlog.${CMTCONFIG}.${CONDDB_implementation}/log.*

  ../$CMTDEB/DetCondExample.exe testXml.opts \
    | sed "s|$DETCONDEXAMPLEROOT|DETCONDEXAMPLEROOT|" \
    > ../job/NEWlog.${CMTCONFIG}.${CONDDB_implementation}/log.testXml

  source ../cmt/dropDB.csh  -tag=$CMTDEB
  source ../cmt/setupDB.csh -tag=$CMTDEB
  ../$CMTDEB/DetCondExample.exe fillCondDB.opts \
    | sed "s|$DETCONDEXAMPLEROOT|DETCONDEXAMPLEROOT|" \
    > ../job/NEWlog.${CMTCONFIG}.${CONDDB_implementation}/log.fillCondDB_1
  ../$CMTDEB/DetCondExample.exe fillCondDB.opts \
    | sed "s|$DETCONDEXAMPLEROOT|DETCONDEXAMPLEROOT|" \
    > ../job/NEWlog.${CMTCONFIG}.${CONDDB_implementation}/log.fillCondDB_2

  ../$CMTDEB/DetCondExample.exe testCondDB.opts \
    | sed "s|$DETCONDEXAMPLEROOT|DETCONDEXAMPLEROOT|" \
    > ../job/NEWlog.${CMTCONFIG}.${CONDDB_implementation}/log.testCondDB

  ../$CMTDEB/DetCondExample.exe daqCondDB.opts \
    | sed "s|$DETCONDEXAMPLEROOT|DETCONDEXAMPLEROOT|" \
    > ../job/NEWlog.${CMTCONFIG}.${CONDDB_implementation}/log.daqCondDB

  unset echo

end
unset theImpls

#-----------------------------------------------------------------------------


