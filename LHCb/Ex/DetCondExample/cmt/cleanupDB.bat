@echo off
rem $Id: cleanupDB.bat,v 1.3 2002-03-28 13:29:30 andreav Exp $
if "%CONDDB_implementation%." == "." ( 
  echo "ERROR: the environment variable CONDDB_implementation is not set"
) ELSE if "%CONDDB_implementation%" == "CondDBOracle" (
  call cleanupDB_Oracle.bat
) ELSE (
  echo "CONDDB_implementation is set to %CONDDB_implementation%"
  echo "ERROR: the only valid value is CondDBOracle!!!"
)

