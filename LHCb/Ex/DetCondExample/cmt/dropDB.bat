@echo off
rem $Id: dropDB.bat,v 1.2 2002-12-03 17:38:41 andreav Exp $
if "%CONDDB_implementation%." == "." ( 
  echo ERROR: the environment variable CONDDB_implementation is not set
  echo Please "set CONDDB_implementation=<value>" and try again
  echo Valid values are {CondDBOracle, CondDBMySQL}
) ELSE if "%CONDDB_implementation%" == "CondDBOracle" (
  call %DETCONDEXAMPLEROOT%\cmt\dropDB_Oracle.bat
) ELSE if "%CONDDB_implementation%" == "CondDBMySQL" (
  call %DETCONDEXAMPLEROOT%\cmt\dropDB_MySQL.bat
) ELSE (
  echo CONDDB_implementation is set to %CONDDB_implementation%
  echo ERROR: valid values are {CondDBOracle, CondDBMySQL}
)

