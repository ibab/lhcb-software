#!/usr/local/bin/tcsh
# $Id: dropDB.csh,v 1.3 2002-12-04 14:40:51 andreav Exp $
if ( ${?CONDDB_implementation} != 1 ) then
  echo "ERROR! Environment variable CONDDB_implementation is not set yet"
  echo "Please 'source ../cmt/setup.csh -tag=$CMTDEB' and try again"
  exit 1
else if ( "${CONDDB_implementation}" == "CondDBObjy" ) then
  source ${DETCONDEXAMPLEROOT}/cmt/dropDB_Objy.csh
  exit 0
else if ( "${CONDDB_implementation}" == "CondDBOracle" ) then
  source ${DETCONDEXAMPLEROOT}/cmt/dropDB_Oracle.csh
  exit 0
else if ( "${CONDDB_implementation}" == "CondDBMySQL" ) then
  source ${DETCONDEXAMPLEROOT}/cmt/dropDB_MySQL.csh
  exit 0
else
  echo "ERROR! CONDDB_implementation is set to ${CONDDB_implementation}"
  echo "Valid values are {CondDBOracle, CondDBObjy, CondDBMySQL}"
  exit 1
endif

