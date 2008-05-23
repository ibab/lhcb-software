@echo off

set tmpzzdffile="%TEMP%\SetupProject_tmpsetup.bat"

python %~d0\%~p0\AllProjectsSetup.py --shell=bat --output=%tmpzzdffile% %1 %2 %3 %4 %5 %6 %7 %8 %9

set AllProjectsSetupStatus=%ERRORLEVEL%

if %AllProjectsSetupStatus% EQU 0 (
	call %tmpzzdffile%
)

if exist %tmpzzdffile% del %tmpzzdffile%
set tmpzzdffile=
set AllProjectsSetupStatus=

