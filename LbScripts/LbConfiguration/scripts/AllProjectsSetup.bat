@echo off

set AllProjectSetup_tmpfile="%TEMP%\AllProjectSetup_tmpsetup.bat"

python %~d0\%~p0\AllProjectsSetup.py --shell=bat --output=%AllProjectSetup_tmpfile%

set AllProjectsSetupStatus=%ERRORLEVEL%

if %AllProjectsSetupStatus% EQU 0 (
	call %AllProjectSetup_tmpfile%
)

if exist %AllProjectSetup_tmpfile% del %AllProjectSetup_tmpfile%
set AllProjectSetup_tmpfile=
set AllProjectsSetupStatus=

