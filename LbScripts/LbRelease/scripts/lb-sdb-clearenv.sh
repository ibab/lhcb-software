#!/bin/bash

#
# Script to remove the SSO cookie after running
#
#

if [ -n $cookie_file ]; then
    if [ -f $cookie_file ]; then
	#echo "Removing the cookie file: $cookie_file"
        rm -f $cookie_file
    fi
fi


