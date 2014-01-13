set do_dbg=
set UTGID=EvtServ
start "%UTGID%" %gaudi_exe% -main=%OPTS%/Main.opts -opt=./EventServer.opts       -msgsvc=%msg_svc% %do_dbg%
sleep 4
set UTGID=EvtCons1
start "%UTGID%" %gaudi_exe% -main=%OPTS%/Main.opts -opt=./EvtCons.opts       -msgsvc=%msg_svc% %do_dbg%
set UTGID=EvtCons2
start "%UTGID%" %gaudi_exe% -main=%OPTS%/Main.opts -opt=./EvtCons.opts       -msgsvc=%msg_svc% %do_dbg%
set UTGID=EvtCons3
start "%UTGID%" %gaudi_exe% -main=%OPTS%/Main.opts -opt=./EvtCons.opts       -msgsvc=%msg_svc% %do_dbg%
set UTGID=EvtCons4
start "%UTGID%" %gaudi_exe% -main=%OPTS%/Main.opts -opt=./EvtCons.opts       -msgsvc=%msg_svc% %do_dbg%
echo %GAUDIONLINEROOT%\%CMTCONFIG%\Gaudi.exe GaudiOnline mep_producer -n=prod_0 -p=333 -s=500 -r=2

