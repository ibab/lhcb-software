#!/bin/csh

#
# Force the CMTEXTRATAGS for SLC 5.8 and posterior 
# 
if ( ! $?CMTEXT_CHECKDONE ) then
setenv CMTEXT_CHECKDONE yes
if ( ! $?CMTEXTRATAGS ) then
    if ( -e /etc/redhat-release ) then
        set res=`sed -e 's/Scientific Linux CERN SLC release \([0-9\.]*\).*/\1/i' /etc/redhat-release` 
    	switch("$res")
 	    case 5.8:
	    case 5.9:
	    case 5.10:
	       #echo "Forcing CMTEXTRATAGS=host-slc5"
	       setenv CMTEXTRATAGS "host-slc5"
	       breaksw
	endsw
    endif
endif
endif

# Adding EOS to the path for installations on AFS
set EOS_LHCB=/afs/cern.ch/project/eos/installation/lhcb
if ( -e  "${EOS_LHCB}" ) then
    source ${EOS_LHCB}/etc/setup.csh
    #setenv PATH ${PATH}:${EOS_LHCB}/bin
    #setenv LD_LIBRARY_PATH ${LD_LIBRARY_PATH}:${EOS_LHCB}/lib64
    #setenv EOS_MGM_URL root://eoslhcb.cern.ch
endif
