@echo off

set AllProjectSetup_tmpfile="%TEMP%\AllProjectSetup_tmpsetup.bat"

runpy LbConfiguration.AllProjectsSetup --shell=bat --output=%AllProjectSetup_tmpfile%

set AllProjectsSetupStatus=%ERRORLEVEL%

if %AllProjectsSetupStatus% EQU 0 (
	call %AllProjectSetup_tmpfile%
)

if exist %AllProjectSetup_tmpfile% del %AllProjectSetup_tmpfile%
set AllProjectSetup_tmpfile=
set AllProjectsSetupStatus=

