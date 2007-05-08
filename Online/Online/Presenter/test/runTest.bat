rem @echo off
REM Clean old processes
taskKill /F /IM DNS.exe
taskKill /F /IM Ctest.exe

set onlineEnvVersion=v3r2
rem set onlineEnvVersion=%1
call \\cern.ch\dfs\Experiments\Lhcb\scripts\lhcbsetup.bat
call \\cern.ch\dfs\Experiments\Lhcb\scripts\setenvProject.bat Online %onlineEnvVersion%
rem call s:\Lhcb\scripts\SetupProject.bat Online %onlineEnvVersion%

REM Setup environment variables
set DIM_DNS_NODE=%COMPUTERNAME%
call %LHCb_release_area%\ONLINE\ONLINE_%onlineEnvVersion%\OnlineSys\%onlineEnvVersion%\cmt\setup.bat
REM Fire up DIM DNS
start /b /low %DIMROOT%\%CMTCONFIG%\DNS.exe

REM Get service components running
start /b /low %CCPCHISTROOT%\%CMTCONFIG%\Ctest.exe
