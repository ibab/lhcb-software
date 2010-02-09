@echo off

rem The :eof label is special, see http://commandwindows.com/batchfiles-branching.htm
if not "%LB_NO_STRIP_PATH%" == "" goto :eof

set StripPath_tmpfile="%TEMP%\StripPath_tmpsetup.bat"

call runpy LbScriptsPolicy.PathStripper --shell=bat --output=%StripPath_tmpfile% -e PATH -e PYTHONPATH -e JOBOPTSEARCHPATH -e HPATH

set StripPathStatus=%ERRORLEVEL%

if %StripPathStatus% EQU 0 (
	call %StripPath_tmpfile%
)


if exist %StripPath_tmpfile% del %StripPath_tmpfile%
set StripPath_tmpfile=
set LB_NO_STRIP_PATH=
