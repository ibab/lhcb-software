@echo off

set tmpzzfile="%TEMP%\SetupProject_tmpsetup.bat"

python %~d0\%~p0\SetupProject.py --shell=bat --output=%tmpzzfile% %1 %2 %3 %4 %5 %6 %7 %8 %9

set SetupProjectStatus=%ERRORLEVEL%

if %SetupProjectStatus% EQU 0 (
	call %tmpzzfile%
)

if exist %tmpzzfile% del %tmpzzfile%
set tmpzzfile=

