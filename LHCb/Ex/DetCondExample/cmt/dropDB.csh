#!/usr/local/bin/tcsh
# $Id: dropDB.csh,v 1.2 2002-12-03 17:38:41 andreav Exp $
if ( ${?CONDDB_implementation} != 1 ) then
  echo "ERROR: the environment variable CONDDB_implementation is not set"
  echo "Please 'setenv CONDDB_implementation <value>' and try again"
  echo "Valid values are {CondDBObjy, CondDBOracle, CondDBMySQL}"
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
  echo "CONDDB_implementation is set to ${CONDDB_implementation}"
  echo "ERROR: valid values are {CondDBOracle, CondDBObjy, CondDBMySQL}"
  exit 1
endif

