#!/bin/sh

#
# Setting CMTEXTRATAGS on SLC 5.8 and later 
#
if [ ! -n "$CMTEXTRATAGS" ]; then
     echo "Checking CMTEXTRATAGS"
     if [ -f /etc/redhat-release ]; then
        res=`sed -e 's/Scientific Linux CERN SLC release \([0-9\.]*\).*/\1/i' /etc/redhat-release` 
	case "$res" in
	    5.8|5.9|5.10)
	        echo "Setting CMTEXTRATAGS"
	    	export CMTEXTRATAGS="host-slc5"
	    	;;
            *)
                ;;
        esac
    fi
fi
