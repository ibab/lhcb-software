alias srun='killall Gaudi.exe; killall test.exe; . ./meprx.sh; sudo ../$CMTCONFIG/test.exe meprx'
alias grun='killall Gaudi.exe; killall test.exe; . ./meprx.sh; $GAUDIONLINEROOT/$CMTCONFIG/Gaudi.exe libGaudiOnline.so mep2event_prod -n=evtprod_0 -r=1 &' 
