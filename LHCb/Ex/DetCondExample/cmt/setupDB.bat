@echo off
rem $Id: setupDB.bat,v 1.4 2002-03-28 13:40:03 andreav Exp $
if "%CONDDB_implementation%." == "." ( 
  echo "The environment variable CONDDB_implementation is not set"
  echo "This should never happen: check your requirements file(s)!"
) ELSE if "%CONDDB_implementation%" == "CondDBOracle" (
  call %DETCONDEXAMPLEROOT%\cmt\setupDB_Oracle.bat
) ELSE (
  echo "CONDDB_implementation is set to %CONDDB_implementation%"
  echo "ERROR: the only valid value is CondDBOracle!!!"
)

