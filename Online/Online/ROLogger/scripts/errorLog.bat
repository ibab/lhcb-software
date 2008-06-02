@echo off
rem . $DATAFLOWDIR/cmtuser/Online_v4r9/Online/ROLogger/cmt/setup.vars.opt
rem
set DIM_DNS_NODE=ecs03
set DIM_DNS_NODE=pclhcb159
set TAN_PORT=YES
set TAN_NODE=%COMPUTERNAME%
gentest.exe ROLogger start_rologger_win32
