
start /b xterm -132 -geometry 150x45  -title ErrorLogger  -e /bin/bash -l -c "export UTGID=ErrorLogger; cd J:/Gaudi/Online/UPI/cmt; ./upi.sh ../../GaudiOnline/win32_vc71_dbg/gaudi.exe GaudiOnline OnlineTask -opts=../../GaudiUPI/options/Errlog.opts -msgsvc=LHCb::DimMessageSvc -auto"
