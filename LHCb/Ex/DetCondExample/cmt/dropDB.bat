@echo off
rem $Id: dropDB.bat,v 1.3 2002-12-04 14:40:51 andreav Exp $
if "%CONDDB_implementation%." == "." ( 
  echo ERROR! Environment variable CONDDB_implementation is not set yet
  echo Please "..\cmt\setup" and try again
) ELSE if "%CONDDB_implementation%" == "CondDBOracle" (
  call %DETCONDEXAMPLEROOT%\cmt\dropDB_Oracle.bat
) ELSE if "%CONDDB_implementation%" == "CondDBMySQL" (
  call %DETCONDEXAMPLEROOT%\cmt\dropDB_MySQL.bat
) ELSE (
  echo ERROR! CONDDB_implementation is set to %CONDDB_implementation%
  echo Valid values are {CondDBOracle, CondDBMySQL}
)

