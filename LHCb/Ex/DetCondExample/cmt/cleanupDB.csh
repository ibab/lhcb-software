#!/usr/local/bin/tcsh
# $Id: cleanupDB.csh,v 1.3 2002-03-28 13:29:30 andreav Exp $
if ( ${?CONDDB_implementation} != 1 ) then
  echo "ERROR: the environment variable CONDDB_implementation is not set"
  exit 1
else if ( "${CONDDB_implementation}" == "CondDBOracle" ) then
  source cleanupDB_Oracle.csh
  exit 0
else if ( "${CONDDB_implementation}" == "CondDBObjy" ) then
  source cleanupDB_Objy.csh
  exit 0
else
  echo "CONDDB_implementation is set to ${CONDDB_implementation}"
  echo "ERROR: the only valid values are CondDBOracle and CondDBObjy!!!"
  exit 1
endif

