#!/usr/local/bin/tcsh -f 
# $Id: RunMixedTest.csh,v 1.1 2002-12-04 14:41:21 andreav Exp $
if ( 1 != ${?DETCONDEXAMPLEROOT} ) then
  echo "ERROR! You must 'source ../cmt/setup.csh -tag=$CMTDEB' first"
  exit 1
endif

if ( "${CMTCONFIG}" == "rh61_gcc2952" ) then
  echo "WARNING! There are no mixed Linux/Win32 tests for RH61"
  exit 0
else
  set theImpls = "CondDBOracle CondDBMySQL"
endif

foreach theImpl ( $theImpls )

  cd ${DETCONDEXAMPLEROOT}/options
  setenv CONDDB_implementation $theImpl
  source ../cmt/setup.csh -tag=$CMTDEB

  set echo
  mkdir -p ../job/NEWlog.${CMTCONFIG}.${CONDDB_implementation}.dbWIN32
  rm -f    ../job/NEWlog.${CMTCONFIG}.${CONDDB_implementation}.dbWIN32/log.*

  if ( "$CONDDB_implementation" == "CondDBOracle" ) then
    setenv DETCONDEXAMPLE_DBNAME MYCONDDB_WIN32
  else 
    setenv DETCONDEXAMPLE_DBNAME test_conddb_win32
  endif

  ../$CMTDEB/DetCondExample.exe testCondDB.opts \
   | sed "s|$DETCONDEXAMPLEROOT|DETCONDEXAMPLEROOT|" \
   > ../job/NEWlog.${CMTCONFIG}.${CONDDB_implementation}.dbWIN32/log.testCondDB

   unset echo

end
unset theImpls

#-----------------------------------------------------------------------------


