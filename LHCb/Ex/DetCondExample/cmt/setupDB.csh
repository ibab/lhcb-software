#!/usr/local/bin/tcsh
# $Id: setupDB.csh,v 1.8 2002-12-04 14:40:51 andreav Exp $
if ( ${?CONDDB_implementation} != 1 ) then
  echo "WARNING! Environment variable CONDDB_implementation is not set yet"
  if ( "${CMTCONFIG}" == "rh61_gcc2952" ) then
    setenv CONDDB_implementation CondDBObjy
  else
    setenv CONDDB_implementation CondDBOracle
  endif
  echo "The following default value will be set:"
  echo "  setenv CONDDB_implementation $CONDDB_implementation"
  echo "Please change it according to your needs if necessary"
  if ( "${CMTCONFIG}" == "rh61_gcc2952" ) then
    echo "Valid values are {CondDBObjy}"
  else
    echo "Valid values are {CondDBOracle, CondDBMySQL}"
  endif
  echo "NB: 'source ../cmt/setup.csh -tag=$CMTDEB' again if you make changes!"
  echo "---------------------------------------------------------------------"
endif
if ( "${CONDDB_implementation}" == "CondDBObjy" ) then
  source ${DETCONDEXAMPLEROOT}/cmt/setupDB_Objy.csh
  exit 0
else if ( "${CONDDB_implementation}" == "CondDBOracle" ) then
  source ${DETCONDEXAMPLEROOT}/cmt/setupDB_Oracle.csh
  exit 0
else if ( "${CONDDB_implementation}" == "CondDBMySQL" ) then
  source ${DETCONDEXAMPLEROOT}/cmt/setupDB_MySQL.csh
  exit 0
else
  echo "ERROR! CONDDB_implementation is set to ${CONDDB_implementation}"
  echo "Valid values are {CondDBOracle, CondDBObjy, CondDBMySQL}"
  exit 1
endif

