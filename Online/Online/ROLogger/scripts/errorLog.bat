@echo on
python.exe -c "import os;print 'SET UTGID='+str(os.getpid())">%TMP%\mypid.bat
call %TMP%\mypid.bat
rm %TMP%\mypid.bat
rem . $DATAFLOWDIR/cmtuser/Online_v4r8/Online/ROLogger/cmt/setup.vars.opt
rem
set DIM_DNS_NODE=ecs03
set DIM_DNS_NODE=pclhcb159
set TAN_PORT=YES
set TAN_NODE=%COMPUTERNAME%

start .\bla.bat 25 132 ROLogger romon_display_server "-service=%*"

rem xterm -ls -132 \
rem     -geometry 210x45+0+0 -bg black -fg white \
rem     -title "${UTGID}:Message display" \
rem     -e "set UTGID=${UTGID}_display; exec -a \${UTGID} `which gentest.exe` libROLogger.so romon_logger -service=\$UTGID -col -buff=0 -disp"&
rem xterm -ls -132 \
rem     -geometry 144x27+0+610 -bg black -fg white \
rem     -title "${UTGID}:History display" \
rem     -e "set UTGID=${UTGID}_history; exec -a \${UTGID} `which gentest.exe` libROLogger.so romon_logger -service=\$UTGID -col -buff=500000"&
rem echo $*
rem sleep 1;
rem xterm -ls -132 \
rem     -geometry 60x27+950+610 \
rem     -title "${UTGID}:Error logger" \
rem     -e "set UTGID=${UTGID}; exec -a \${UTGID} `which gentest.exe` libROLogger.so romon_display_server -service=$*" &
