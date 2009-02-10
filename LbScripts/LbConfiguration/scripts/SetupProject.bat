@echo off

set SetupProject_tmpfile="%TEMP%\SetupProject_tmpsetup.bat"

call runpy LbConfiguration.SetupProject --shell=bat --output=%SetupProject_tmpfile% %*

set SetupProjectStatus=%ERRORLEVEL%

if %SetupProjectStatus% EQU 0 (
	call %SetupProject_tmpfile%
)

if exist %SetupProject_tmpfile% del %SetupProject_tmpfile%
set SetupProject_tmpfile=

