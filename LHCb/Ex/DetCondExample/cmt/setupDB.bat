@echo off
rem $Id: setupDB.bat,v 1.6 2002-12-04 14:40:51 andreav Exp $
if "%CONDDB_implementation%." == "." ( 
  echo WARNING! Environment variable CONDDB_implementation is not set yet
  set CONDDB_implementation=CondDBOracle
  set doEcho=yes
) else (
  set doEcho=no
)  
if "%doEcho%" == "yes" ( 
  echo The following default value will be set:
  echo   set CONDDB_implementation=%CONDDB_implementation%
  echo Please change it according to your needs if necessary
  echo NB: "..\cmt\setup" again if you make changes!
  echo ---------------------------------------------------------------------
)
set doEcho=

if "%CONDDB_implementation%" == "CondDBOracle" (
  call %DETCONDEXAMPLEROOT%\cmt\setupDB_Oracle.bat
) else if "%CONDDB_implementation%" == "CondDBMySQL" (
  call %DETCONDEXAMPLEROOT%\cmt\setupDB_MySQL.bat
) else (
  echo CONDDB_implementation is set to %CONDDB_implementation%
  echo ERROR: valid values are {CondDBOracle, CondDBMySQL}
)

