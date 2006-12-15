set breakpoint pending on
define meprx
run libGaudiOnline.so OnlineTask -msgsvc=LHCb::DimMessageSvc -auto -opt=../options/MEPRxSvc.opts
end

define readmbm
run libGaudiOnline.so OnlineTask -msgsvc=LHCb::DimMessageSvc -auto -opt=../../options/ReadMBM.opts
end

define sender
run libGaudiOnline.so OnlineTask -msgsvc=LHCb::DimMessageSvc -auto -opt=../../options/MDFSender.opts
end

define receiver
run libGaudiOnline.so OnlineTask -msgsvc=LHCb::DimMessageSvc -auto -opt=../../options/MDFReceiver.opts
end

define writer
run libGaudiOnline.so OnlineTask -msgsvc=LHCb::DimMessageSvc -auto -opt=../../options/DiskWR.opts
end

define holder
run libGaudiOnline.so OnlineTask -msgsvc=LHCb::DimMessageSvc -auto -opt=../../options/MEPHolder.opts
end

define evtprod
run libGaudiOnline.so OnlineTask -msgsvc=LHCb::DimMessageSvc -auto -opt=../../options/MEPConverter.opts
end

define errserv
run libGaudiOnline.so OnlineTask -msgsvc=LHCb::DimMessageSvc -auto -main=../../options/ErrorSrv.opts -opt=../../options/Daemon.opts
end

define errlog
run libGaudiOnline.so OnlineTask -msgsvc=MessageSvc -auto -opt=../../options/ErrorLogger.opts
end

define mbmmon
run libGaudiOnline.so OnlineStart libOnlineKernel.so mbm_mon
end

define Recv_0
run libGaudiOnline.so OnlineTask -msgsvc=MessageSvc -auto -opt=../../options/NetReceiver.opts
end

define Send0_0
run libGaudiOnline.so OnlineTask -msgsvc=MessageSvc -auto -opt=../../options/NetSend_0.opts
end
