#!/bin/bash

#
# Script to remove the SSO cookie after running
#
#

if [ -n $cookie_file ]; then
    if [ -f $cookie_file ]; then
        # Only remove the cookie if the variable LB_SDB_REMOVE_COOKIE was set
	if [ -n "${LB_SDB_REMOVE_COOKIE}" ]; then
	    rm -f $cookie_file
	fi
    fi
fi


