rem $Id: RunTests.bat,v 1.2 2003-01-31 21:15:34 andreav Exp $
@echo off
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

rem # YOU MUST SET YOUR DETCONDEXAMPLEROOT WITH DOUBLE BACKSLASHES HERE!
@echo on
mkdir ..\job\NEWlog.%CMTCONFIG%.%CONDDB_implementation%
del ..\job\NEWlog.%CMTCONFIG%.%CONDDB_implementation%\log.*
cd %DETCONDEXAMPLEROOT%\options

run DetCondExample testXml.opts | sed "s|I:\\\\newmycmt11\\\\Ex\\\\DetCondExample\\\\v4r3\\\\|DETCONDEXAMPLEROOT/|" > ..\job\NEWlog.%CMTCONFIG%.%CONDDB_implementation%\log.testXml
@echo off

call ..\cmt\dropDB.bat
@echo on
run DetCondExample fillCondDB.opts | sed "s|I:\\\\newmycmt11\\\\Ex\\\\DetCondExample\\\\v4r3\\\\|DETCONDEXAMPLEROOT/|" > ..\job\NEWlog.%CMTCONFIG%.%CONDDB_implementation%\log.fillCondDB_1
run DetCondExample fillCondDB.opts | sed "s|I:\\\\newmycmt11\\\\Ex\\\\DetCondExample\\\\v4r3\\\\|DETCONDEXAMPLEROOT/|" > ..\job\NEWlog.%CMTCONFIG%.%CONDDB_implementation%\log.fillCondDB_2

run DetCondExample testCondDB.opts | sed "s|I:\\\\newmycmt11\\\\Ex\\\\DetCondExample\\\\v4r3\\\\|DETCONDEXAMPLEROOT/|" > ..\job\NEWlog.%CMTCONFIG%.%CONDDB_implementation%\log.testCondDB

run DetCondExample daqCondDB.opts | sed "s|I:\\\\newmycmt11\\\\Ex\\\\DetCondExample\\\\v4r3\\\\|DETCONDEXAMPLEROOT/|" > ..\job\NEWlog.%CMTCONFIG%.%CONDDB_implementation%\log.daqCondDB
@echo off

cd %DETCONDEXAMPLEROOT%\job

if "%CONDDB_implementation%" == "CondDBMySQL" goto exit
goto next

:exit
