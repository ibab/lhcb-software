rem Setting DetCondExample v2r1 in I:\newmycmt\Ex
@echo off
set CMTROOT=E:\CMT\v1r10p20011126
call %CMTROOT%\mgr\setup.bat

set tempfile=%HOMEDRIVE%%HOMEPATH%tmpsetup.bat
%CMTROOT%\%CMTBIN%\cmt.exe -quiet setup -bat -pack=DetCondExample -version=v2r1 -path=I:\newmycmt\Ex %1 %2 %3 %4 %5 %6 %7 %8 %9 >%tempfile%
if exist %tempfile% call %tempfile%
if exist %tempfile% del %tempfile%
