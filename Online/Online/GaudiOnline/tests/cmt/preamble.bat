set TAN_PORT=YES
set TAN_NODE=%COMPUTERNAME%.cern.ch
set DIM_DNS_NODE=%COMPUTERNAME%.cern.ch
rem
set OPTS=%GAUDIONLINEROOT%/options
set gaudi_exe=%GAUDIONLINEROOT%\%CMTCONFIG%\Gaudi.exe GaudiOnline.dll OnlineTask -auto
set msg_svc=LHCb::DimMessageSvc
set msg_svc=MessageSvc
