#!/bin/bash

#
# Script to prepare the environment for the software configuration db
#
#

# Bash function to setup the Ariadne environment silently
setup_env () {
    . SetupProject.sh --silent Tools --use Ariadne

    if [ "$SetupProjectStatus" != 0 ] ; then
        echo "ERROR: Cannot prepare the environment to use Ariadne."
        exit 1
    fi
}


# Checking the age of the cookie file
# Removing it if too old
export cookie_file=/tmp/$USER/ssocookie-ariadne.txt
if [ -f ${cookie_file}  ]; then
    export cmd="import os; import time; age=(time.time()-os.path.getctime('${cookie_file}')); print int(age);"
    age=$(python -c "${cmd}")

    if [ "${age}" -gt "3600" ]; then
	# Old cookie file, we'll remove it and get a new cookie
	rm -f ${cookie_file}
    fi
fi

if [ ! -f ${cookie_file}  ]; then
    # Checking whether we have the cern-get-sso-cookie  
    command -v cern-get-sso-cookie >/dev/null 2>&1 || { echo >&2 "I need 'cern-get-sso-cookie' package to run but it's not installed. Aborting."; exit 1; }

    # Now actually getting it... 
    cern-get-sso-cookie --krb -u https://ariadne-lhcb.cern.ch/ --reprocess -o $cookie_file
    if [ $? != 0 ] ; then
	printf "WARNING: Error encountered while acquiring CERN SSO cookie for Ariadne. Attempting to connect with no SSO..\n\n"
    fi    
fi

# And setting the env
setup_env
