set UTGID= eee
rem @echo off
REM Clean old processes
taskkill /F /IM Gaudi.exe
taskKill /F /IM DNS.exe

set onlineEnvVersion="v3r1"
rem set onlineEnvVersion=%1
call \\cern.ch\dfs\Experiments\Lhcb\scripts\lhcbsetup.bat
call \\cern.ch\dfs\Experiments\Lhcb\scripts\setenvProject.bat Online %onlineEnvVersion%
call s:\Lhcb\scripts\SetupProject.bat Online %onlineEnvVersion%

REM Setup environment variables
set DIM_DNS_NODE=%COMPUTERNAME%

REM Fire up DIM DNS
start /b /low %DIMROOT%\%CMTCONFIG%\DNS.exe

REM Get service components running
set MSGSVGTYPE=MessageSvc
set OPTS=%GAUCHOJOBROOT%\options\GauchoJob.opts 
start /b %GAUDIEXE% %GAUDIONLINEROOT%\%CMTCONFIG%/GaudiOnline.dll GaudiOnline -runable=LHCb::OnlineRunable -options=..\GaudiExample.opts -msgsvc=MessageSvc -loop -auto

