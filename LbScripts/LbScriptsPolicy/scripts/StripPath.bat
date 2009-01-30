@echo off

set StripPath_tmpfile="%TEMP%\StripPath_tmpsetup.bat"

runpy LbScriptsPolicy.PathStripper --shell=bat --output=%StripPath_tmpfile% -e PATH -e PYTHONPATH -e JOBOPTSEARCHPATH -e HPATH

call %StripPath_tmpfile%


if exist %StripPath_tmpfile% del %StripPath_tmpfile%
set StripPath_tmpfile=

