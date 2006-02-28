set breakpoint pending on
define rmeprod
	run libGaudiOnline.so mep2event_prod -n=evtprod_0 -r=1
end
define rrx
	run meprx
end

