rem set UTGID= eee
rem @echo off
REM Clean old processes
rem taskkill /F /IM Gaudi.exe
taskKill /F /IM DNS.exe
taskKill /F /IM Ctest.exe

set onlineEnvVersion="v3r2"
rem set onlineEnvVersion=%1
call \\cern.ch\dfs\Experiments\Lhcb\scripts\lhcbsetup.bat
call \\cern.ch\dfs\Experiments\Lhcb\scripts\setenvProject.bat Online %onlineEnvVersion%
call s:\Lhcb\scripts\SetupProject.bat Online %onlineEnvVersion%
call z:/cern.ch/lhcb/software/releases/ONLINE/ONLINE_v3r2/OnlineSys/v3r2/cmt/setup.bat

REM Setup environment variables
set DIM_DNS_NODE=%COMPUTERNAME%

REM Fire up DIM DNS
start /b /low %DIMROOT%\%CMTCONFIG%\DNS.exe

REM Get service components running
start /b /low %CCPCHISTROOT%\%CMTCONFIG%\Ctest.exe

