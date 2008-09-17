@echo off

set LbLogin_tmpfile="%TEMP%\LbLogin_tmpsetup.bat"

python %~d0\%~p0\LbLogin.py --shell=bat --output=%LbLogin_tmpfile% %1 %2 %3 %4 %5 %6 %7 %8 %9

set LbLoginStatus=%ERRORLEVEL%

if %LbLoginStatus% EQU 0 (
	call %LbLogin_tmpfile%
)

if exist %LbLogin_tmpfile% del %LbLogin_tmpfile%
set LbLogin_tmpfile=

