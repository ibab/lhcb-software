rem $Id: RunMixedTest.bat,v 1.1 2002-12-04 14:41:21 andreav Exp $
echo off
if "%DETCONDEXAMPLEROOT%." == "." (
  echo ERROR! You must '..\cmt\setup' first"
  goto exit
) 

set CONDDB_implementation=CondDBOracle
goto run

:next
if "%CONDDB_implementation%" == "CondDBOracle" set CONDDB_implementation=CondDBMySQL

:run
echo CONDDB_implementation is %CONDDB_implementation%
call ..\cmt\setup.bat

if "%CONDDB_implementation%" == "CondDBOracle" (
  set DETCONDEXAMPLE_DBNAME=MYCONDDB_LINUX
) else (
  set DETCONDEXAMPLE_DBNAME=test_conddb_linux
)

rem # YOU MUST SET YOUR DETCONDEXAMPLEROOT WITH DOUBLE BACKSLASHES HERE!
@echo on
mkdir ..\job\NEWlog.%CMTCONFIG%.%CONDDB_implementation%.dbLINUX
del ..\job\NEWlog.%CMTCONFIG%.%CONDDB_implementation%.dbLINUX\log.*
cd %DETCONDEXAMPLEROOT%\options
run DetCondExample testCondDB.opts | sed "s|I:\\\\newmycmt11\\\\Ex\\\\DetCondExample\\\\v4r2\\\\|DETCONDEXAMPLEROOT/|" > ..\job\NEWlog.%CMTCONFIG%.%CONDDB_implementation%.dbLINUX\log.testCondDB
@echo off
cd %DETCONDEXAMPLEROOT%\job

if "%CONDDB_implementation%" == "CondDBMySQL" goto exit
goto next

:exit
