@echo off

set SetupProject_tmpfile="%TEMP%\SetupProject_tmpsetup.bat"

runpy LbConfiguration.SetupProject --shell=bat --build-env --output=%SetupProject_tmpfile% %1 %2 %3 %4 %5 %6 %7 %8 %9

set SetupProjectStatus=%ERRORLEVEL%

if %SetupProjectStatus% EQU 0 (
	call %SetupProject_tmpfile%
)

if exist %SetupProject_tmpfile% del %SetupProject_tmpfile%
set SetupProject_tmpfile=

