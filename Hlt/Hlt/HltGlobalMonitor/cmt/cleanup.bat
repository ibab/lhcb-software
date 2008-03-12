@echo off
if NOT DEFINED CMTROOT set CMTROOT=Z:\cern.ch\sw\contrib\CMT\v1r19& set PATH=%CMTROOT%\%CMTBIN%;%PATH%& set CMTBIN=VisualC& if not defined CMTCONFIG set CMTCONFIG=%CMTBIN%

set cmttempfile="%TEMP%\tmpsetup.bat"
%CMTROOT%\%CMTBIN%\cmt.exe cleanup -bat  -pack=HltCommon -version=v1r4 -path="%~d0%~p0..\..\.."   %1 %2 %3 %4 %5 %6 %7 %8 %9 >%cmttempfile%
if exist %cmttempfile% call %cmttempfile%
if exist %cmttempfile% del %cmttempfile%
set cmttempfile=

