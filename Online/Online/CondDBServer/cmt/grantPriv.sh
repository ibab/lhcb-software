#!/bin/sh

# This sets ORACLE_HOME and ORACLE_SID (needed?)
. /sw/oracle/linux/bin/oraenv

export CMTCONFIG=slc4_ia32_gcc34
if [ -r /sw/lib/LbLogin.sh ]; then
    . /sw/lib/LbLogin.sh >& /dev/null
else
    echo CMT environment not found
    exit 1
fi

SetupProject Online v4r20 --use Online/CondDBServer >& /dev/null

date >> grantPriv.log
grantPriv.py >> grantPriv.log

#date > test.log
#printenv | sort >> test.log
