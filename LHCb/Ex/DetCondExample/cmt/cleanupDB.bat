@echo off
rem $Id: cleanupDB.bat,v 1.4 2002-03-28 13:40:03 andreav Exp $
if "%CONDDB_implementation%." == "." ( 
  echo "ERROR: the environment variable CONDDB_implementation is not set"
) ELSE if "%CONDDB_implementation%" == "CondDBOracle" (
  call %DETCONDEXAMPLEROOT%\cmt\cleanupDB_Oracle.bat
) ELSE (
  echo "CONDDB_implementation is set to %CONDDB_implementation%"
  echo "ERROR: the only valid value is CondDBOracle!!!"
)

