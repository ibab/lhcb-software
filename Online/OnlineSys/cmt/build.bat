if exist %CMTINSTALLAREA% rd /q/s %CMTINSTALLAREA%
set QUIET=1
cmt broadcast if exist ..\%CMTCONFIG% rd /q/s ..\%CMTCONFIG%
cmt broadcast cmt config
cmt broadcast cmt make > build_%CMTCONFIG%.log

