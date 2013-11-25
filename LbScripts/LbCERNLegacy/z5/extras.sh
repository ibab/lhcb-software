#!/bin/sh

#
# Setting CMTEXTRATAGS on SLC 5.8 and later 
#
if [ ! -n "$CMTEXT_CHKDONE" ]; then
export CMTEXT_CHKDONE=yes
if [ ! -n "$CMTEXTRATAGS" ]; then 
     if [ -f /etc/redhat-release ]; then
        res=`sed -e 's/Scientific Linux CERN SLC release \([0-9\.]*\).*/\1/i' /etc/redhat-release` 
	case "$res" in
	    5.8|5.9|5.10)
	        #echo "Forcing CMTEXTRATAGS=host-slc5"
	    	export CMTEXTRATAGS="host-slc5"
	    	;;
            *)
                ;;
        esac
    fi
fi
fi


# Adding EOS to the path from AFS if available 
EOS_LHCB=/afs/cern.ch/project/eos/installation/lhcb
if [ -d $EOS_LHCB ]; then
    source ${EOS_LHCB}/etc/setup.sh
    #export PATH=${PATH}:${EOS_LHCB}/bin
    #export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${EOS_LHCB}/lib64
    #export EOS_MGM_URL=root://eoslhcb.cern.ch
fi

