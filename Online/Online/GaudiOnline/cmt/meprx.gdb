set breakpoint pending on
define rmeprx
	run libGaudiOnline.so OnlineTask -opt=../options/MEPRxSvc.opts -msgsvc=LHCb::DimMessageSvc -a  
end
define rmepcv
#	b ../src/MEPConverterSvc.cpp:190
#	b ../src/MEPConverterSvc.cpp:150
#	b ../src/RawEventHelpers.cpp:535
#	b ../../OnlineKernel/src/MBM/mbmlib.cpp:1300
	run libGaudiOnline.so OnlineTask -opt=../options/MEPConverter.opts -msgsvc=MessageSvc -a
end
define rrx
	run meprx
end

define rmoore
run libGaudiOnline.so OnlineTask -opt=../options/ReadMBM.opts -msgsvc=MessageSvc -auto
end

